{-# LANGUAGE DuplicateRecordFields #-}
{-# LANGUAGE NamedFieldPuns #-}

import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util

import Data.List
import System.Directory
import qualified System.Directory as D
import System.Environment
import Control.Monad
import System.IO
import Control.Monad.State

import qualified Data.ByteString.Lazy as BS
import Data.Binary.Get

dropDir :: FilePath -> FilePath -> FilePath
dropDir x base = joinPath $ dropDir' (splitPath x) (splitPath base)
  where
    dropDir' :: [FilePath] -> [FilePath] -> [FilePath]
    dropDir' xs [] = xs
    dropDir' (x:xs) (b:bs) =
      let diff = x \\ b
      in if diff == "" || diff == "/" then
          dropDir' xs bs
        else
          error $ "could not drop '"++(joinPath (b:bs))++"' from '"++(joinPath (x:xs))++"'"

listDirRec :: FilePath -> IO [FilePath]
listDirRec path = do
  isDir <- D.doesDirectoryExist path
  if isDir then do
    files <- ((path </>) <$>) <$> listDirectory path
    concat <$> traverse listDirRec files
  else
    return [path]


data DirConfig = DirConfig {
  inDir :: String,
  distDir :: String,
  bldDir :: String,
  depFileDir :: String,
  depsDir:: String,
  clangDBDir :: String
}
emptyDirConfig = DirConfig {
  inDir = "",
  distDir = "",
  bldDir = "",
  depFileDir = "",
  depsDir = "",
  clangDBDir = ""
}
globalDirConfig = dirconfig{
    depFileDir = bldDir dirconfig</>"dep",
    clangDBDir = bldDir dirconfig</>"clang_compdb"
  }
  where
    dirconfig :: DirConfig
    dirconfig = emptyDirConfig{
      inDir = "src",
      distDir = "dist",
      bldDir = "bld",
      depsDir = "deps"
     }
patchDirConfig :: DirConfig -> DirConfig -> DirConfig
patchDirConfig base patch = DirConfig{
    inDir      = patchString (inDir base) (inDir patch),
    distDir    = patchString (distDir base) (distDir patch),
    bldDir     = patchString (bldDir base) (bldDir patch),
    depFileDir = patchString (depFileDir base) (depFileDir patch),
    depsDir    = patchString (depsDir base) (depsDir patch),
    clangDBDir = patchString (clangDBDir base) (clangDBDir patch)
  }
  where
    patchString :: String -> String -> String
    patchString base patch =
      if null patch then base else patch
dirConfigJoinPath :: DirConfig -> FilePath -> DirConfig
dirConfigJoinPath cfg path = DirConfig{
  inDir      = inDir cfg </> path,
  distDir    = distDir cfg </> path,
  bldDir     = bldDir cfg </> path,
  depFileDir = depFileDir cfg </> path,
  depsDir    = depsDir cfg </> path,
  clangDBDir = clangDBDir cfg </> path
}



data IncludeDirList = IncludeDirList {
  systemIncludeDirs :: [FilePath],
  includeDirs :: [FilePath]
}
emptyIncludeDirList :: IncludeDirList
emptyIncludeDirList = IncludeDirList {
  systemIncludeDirs = [],
  includeDirs = []
}

data CPPObjectConfig = CPPObjectConfig {
  name :: String,
  basedir_config :: DirConfig,
  dirconfig_override :: DirConfig,
  debug :: Bool,
  compiler :: String,
  includes :: IncludeDirList,
  common_flags :: [String],
  debug_flags:: [String],
  release_flags :: [String]
}
defaultCPPCommonFlags :: [String]
defaultCPPCommonFlags = ["-std=c++1z"] ++ warnFlags ++ diagFlags
  where
    warnFlags = [
        "-Weverything", "-Wno-float-equal", "-Wno-padded",
        "-Wno-c++98-compat", "-Wno-c++98-c++11-compat", "-Wno-c++98-c++11-compat-pedantic",
        "-Wno-c++98-compat-pedantic", "-Wno-c99-extensions", "-Wno-c++98-c++11-c++14-compat"
      ]
    diagFlags = ["-fcolor-diagnostics"]
defaultCPPDebugFlags :: [String]
defaultCPPDebugFlags = ["-O0"]
defaultCPPReleaseFlags :: [String]
defaultCPPReleaseFlags = ["-Ofast"]

emptyCPPObjectConfig :: CPPObjectConfig
emptyCPPObjectConfig = CPPObjectConfig {
  name = "",
  basedir_config = emptyDirConfig,
  dirconfig_override = emptyDirConfig,
  debug = True,
  compiler = "",
  includes = emptyIncludeDirList,
  common_flags = defaultCPPCommonFlags,
  debug_flags = defaultCPPDebugFlags,
  release_flags = defaultCPPReleaseFlags
}
verifyCPPObjectConfig :: CPPObjectConfig -> ()
verifyCPPObjectConfig cfg =
  if null (name (cfg :: CPPObjectConfig)) then error "Target name cannot be empty" else
    if null (compiler cfg) then error "Compiler cannot be empty" else ()

cppObjectRules :: CPPObjectConfig -> Rules ()
cppObjectRules cfg =
  [
    bldDir patchedDirCfg<//>"*"<.>"o",
    depFileDir patchedDirCfg<//>"*"<.>".dep",
    clangDBDir patchedDirCfg<//>"*"<.>"json"
  ] &%> \[out, dep, clangdb] -> do
    let src = inDir patchedDirCfg</>dropDir out (bldDir patchedDirCfg)-<.>"cpp"
    need [src]

    let command =
          [compiler cfg] ++
          common_flags (cfg :: CPPObjectConfig) ++
          (if debug (cfg :: CPPObjectConfig) then debug_flags (cfg :: CPPObjectConfig) else release_flags (cfg :: CPPObjectConfig)) ++
          (("-isystem"++) <$> systemIncludeDirs (includes (cfg :: CPPObjectConfig))) ++
          (("-I"++) <$> includeDirs (includes (cfg :: CPPObjectConfig))) ++
          ["-o", out]

    () <- cmd command "-M" "-MF" [dep] [src]
    needMakefileDependencies dep

    () <- cmd command "-MJ" [clangdb] "-c" [src]
    return ()

  where
    patchedBasedirCfg :: DirConfig
    patchedBasedirCfg = patchDirConfig globalDirConfig (basedir_config (cfg :: CPPObjectConfig))

    patchedDirCfg :: DirConfig
    patchedDirCfg = patchDirConfig (dirConfigJoinPath patchedBasedirCfg (name (cfg :: CPPObjectConfig))) (dirconfig_override (cfg :: CPPObjectConfig))


data LibConfig = LibConfig {
  otherTargets :: [String],
  system :: [String],
  byPath :: [FilePath]
}
emptyLibConfig :: LibConfig
emptyLibConfig = LibConfig {
  otherTargets = [],
  system = [],
  byPath = []
}

data LinkConfig = LinkConfig {
  outPath :: String,
  basedir_config :: DirConfig,
  dirconfig_override :: DirConfig,
  debug :: Bool,
  linker :: String,
  sources :: [FilePath],
  libraryDirList :: [FilePath],
  libraries :: LibConfig,
  common_flags :: [String],
  debug_flags:: [String],
  release_flags :: [String]
}
defaultLinkCommonFlags :: [String]
defaultLinkCommonFlags = []

emptyLinkConfig :: LinkConfig
emptyLinkConfig = LinkConfig {
  outPath = "",
  basedir_config = emptyDirConfig,
  dirconfig_override = emptyDirConfig,
  debug = True,
  linker = "",
  sources = [],
  libraryDirList = [],
  libraries = emptyLibConfig,
  common_flags = [],
  debug_flags = [],
  release_flags = []
}
verifyLinkConfig :: LinkConfig -> ()
verifyLinkConfig cfg =
  if null (outPath cfg) then error "Out path cannot be empty" else
    if null (linker cfg) then error "Linker cannot be empty" else ()

linkRules :: LinkConfig -> Rules ()
linkRules cfg =
  bldDir patchedBasedirCfg<//>outPath cfg %> \out -> do
    let srcs = (\x -> bldDir patchedBasedirCfg</>x<.>"o") <$> sources cfg
    need srcs

    let command =
          [linker cfg] ++
          common_flags (cfg :: LinkConfig) ++
          (if debug (cfg :: LinkConfig) then debug_flags (cfg :: LinkConfig) else release_flags (cfg :: LinkConfig)) ++
          (("-L"++) <$> libraryDirList cfg) ++
          ((\x -> "-L"++takeDirectory x) <$> byPath (libraries cfg :: LibConfig)) ++
          (("-l"++) <$> (system.libraries $ cfg)) ++
          ((\x -> "-l"++libNameFromPath x) <$> byPath (libraries cfg :: LibConfig)) ++
          ["-o", out]

    () <- cmd command srcs
    return ()

  where
    patchedBasedirCfg :: DirConfig
    patchedBasedirCfg = patchDirConfig globalDirConfig (basedir_config (cfg :: LinkConfig))

    libNameFromPath :: FilePath -> String
    libNameFromPath x = takeFileName x


defaultNasmCommonFlags :: [String]
defaultNasmCommonFlags = ["-Wall"]

defaultNasmDebugFlags :: [String]
defaultNasmDebugFlags = ["-O0"]

defaultNasmReleaseFlags :: [String]
defaultNasmReleaseFlags = ["-O2"]

data NasmConfig = NasmConfig {
  name :: String,
  basedir_config :: DirConfig,
  dirconfig_override :: DirConfig,
  debug :: Bool,
  nasm :: String,
  includes :: [FilePath],
  defines :: [(String, String)],
  common_flags :: [String],
  debug_flags:: [String],
  release_flags :: [String]
}
emptyNasmConfig :: NasmConfig
emptyNasmConfig = NasmConfig {
  name = "",
  basedir_config = emptyDirConfig,
  dirconfig_override = emptyDirConfig,
  debug = True,
  nasm = "",
  includes = [],
  defines = [],
  common_flags = defaultNasmCommonFlags,
  debug_flags = defaultNasmDebugFlags,
  release_flags = defaultNasmReleaseFlags
}
verifyNasmConfig :: NasmConfig -> ()
verifyNasmConfig cfg =
  if null (name (cfg :: NasmConfig)) then error "Target name cannot be empty" else
    if null (nasm cfg) then error "Nasm command cannot be empty" else ()

nasmBuild :: (String, String, String) -> DirConfig -> NasmConfig -> Action ()
nasmBuild (out, dep, src) patchedDirCfg cfg = do
  need [src]
  let command =
        [nasm cfg] ++
        common_flags (cfg :: NasmConfig) ++
        (if debug (cfg :: NasmConfig) then debug_flags (cfg :: NasmConfig) else release_flags (cfg :: NasmConfig)) ++
        ((\x -> "-i" ++ addTrailingPathSeparator x) <$> ((inDir patchedDirCfg):includes (cfg :: NasmConfig))) ++
        ((\(n, v) -> "-D"++n++(if null v then "" else "="++v)) <$> defines cfg) ++
        ["-o", out]

  () <- cmd command "-M" "-MF" [dep] [src]
  needMakefileDependencies dep

  () <- cmd command [src]
  return ()

nasmRules :: NasmConfig -> Rules ()
nasmRules cfg =
  [
    bldDir patchedDirCfg<//>"*"<.>"o",
    depFileDir patchedDirCfg<//>"*"<.>".dep"
  ] &%> \[out, dep] -> do
    let src = inDir patchedDirCfg</>dropDir out (bldDir patchedDirCfg)-<.>"nasm"
    need [src]

    nasmBuild (out, dep, src) patchedDirCfg cfg

  where
    patchedBasedirCfg :: DirConfig
    patchedBasedirCfg = patchDirConfig globalDirConfig (basedir_config (cfg :: NasmConfig))

    patchedDirCfg :: DirConfig
    patchedDirCfg = patchDirConfig (dirConfigJoinPath patchedBasedirCfg (name (cfg :: NasmConfig))) (dirconfig_override (cfg :: NasmConfig))


shake_builddb = bldDir globalDirConfig</>"builddb"
shake_report = bldDir globalDirConfig</>"report"

shakeOptions' :: ShakeOptions
shakeOptions' = shakeOptions{
  shakeFiles = shake_builddb,
  shakeReport = (shake_report </>) <$> [
    "t"<.>"trace", "h"<.>"html"
  ],
  shakeLint = Just LintBasic,
  shakeTimings = True
}


findParttData :: (String, String, String) -> Integer -> String -> StateT (Integer, (Integer, Integer), (Integer, Integer)) IO ()
findParttData (path_kkexec, path_ukexec, path_prttbin) prtt_size x =
  if x == path_prttbin then do
    (fpos, (krnl_loc, krnl_size), (usr_loc, usr_size)) <- get
    put (fpos+prtt_size, (krnl_loc, krnl_size), (usr_loc, usr_size))
  else do
    h <- liftIO $ openFile x ReadMode -- fixme: use withFile somehow

    size <- liftIO $ hFileSize h
    (fpos, (krnl_loc, krnl_size), (usr_loc, usr_size)) <- get

    if x == path_kkexec then
      put (fpos+size, (fpos, size), (usr_loc, usr_size))
    else if x == path_ukexec then
      put (fpos+size, (krnl_loc, krnl_size), (fpos, size))
    else
      put (fpos+size, (krnl_loc, krnl_size), (usr_loc, usr_size))

    liftIO $ hClose h

main :: IO ()
main = shakeArgs shakeOptions' $ do
  liftIO $ createDirectoryIfMissing True shake_report

  let llvmPath = "/Users/maximsmol/opt/llvm/"
  let llvmBin = llvmPath</>"bld/bin/"

  let defaultCPPObjectConfig = emptyCPPObjectConfig{
        compiler=llvmBin</>"clang++",
        common_flags=defaultCPPCommonFlags{-++["-Wpadded"]-}
      }
  let defaultLinkConfig = emptyLinkConfig{
        linker=llvmBin</>"ld.lld"
      }
  let defaultNasmConfig = emptyNasmConfig{
        nasm="nasm"
      }

  cppObjectRules defaultCPPObjectConfig{
    name = "krnl",
    common_flags = common_flags (defaultCPPObjectConfig :: CPPObjectConfig)++["-target", "i386-none-elf", "-mno-sse"]
  }

  let kernelLinkBaseCommonFlags = defaultLinkCommonFlags++["-nostdlib", "--Ttext", "0"]
  let kernelLinkBaseConfig = defaultLinkConfig{
    common_flags = kernelLinkBaseCommonFlags,
    libraries = emptyLibConfig{
      byPath = [llvmPath</>"compiler-rt_x86-none-elf/lib/generic/clang_rt.builtins-i386"]
    },
    sources = ("krnl"</>) <$> ["main", "ata", "term"]
  }
  linkRules kernelLinkBaseConfig{
    outPath = "krnl"</>"elf",
    common_flags = kernelLinkBaseCommonFlags
  }
  linkRules kernelLinkBaseConfig{
    outPath = "krnl"</>"bin",
    common_flags = kernelLinkBaseCommonFlags++["--oformat", "binary"]
  }

  bldDir globalDirConfig</>"krnl"</>"kmain_addr" %> \out -> do
    let krnlelf = bldDir globalDirConfig</>"krnl"</>"elf"
    need [krnlelf]
    () <- cmd Shell (llvmBin</>"llvm-objdump") "-t" [krnlelf] "|" "grep kmain" "|" "ggrep -oP \"^[a-f0-9]+\"" "|" "tac" "-rs" ".." "|" "xxd" "-r" "-p" ">" [out]
    return ()

  bldDir globalDirConfig</>"krnl"</>"kexec" %> \out -> do
    let srcs = (\x -> bldDir globalDirConfig</>"krnl"</>x)<$>["kmain_addr", "bin"]
    need srcs
    () <- cmd Shell "printf" "kexec" ">" [out]
    () <- cmd Shell "cat" srcs ">>" [out]
    return ()


  let img_srcs = (bldDir globalDirConfig</>) <$> [
          "boot"</>"boot"<.>"o",
          "prtt"</>"bin",
          "krnl"</>"kexec"
        ]

  nasmRules defaultNasmConfig{
    name = "boot"
  }

  let prttDirConfig = dirConfigJoinPath globalDirConfig "prtt"
  [bldDir prttDirConfig</>"size", depFileDir prttDirConfig</>"size"<.>"dep"] &%> \[out, dep] -> do
    let src = inDir prttDirConfig</>"size"<.>"nasm"
    nasmBuild (out, dep, src) prttDirConfig defaultNasmConfig
  [bldDir prttDirConfig</>"bin", depFileDir prttDirConfig</>"gen"<.>"dep"] &%> \[out, dep] -> do
    let src = inDir prttDirConfig</>"gen"<.>"nasm"
    need $ img_srcs \\ [out]

    need [bldDir prttDirConfig</>"size"]
    prtt_sizef_raw <- liftIO . BS.readFile $ bldDir prttDirConfig</>"size"
    let prtt_size = runGet getWord32le prtt_sizef_raw

    let f = forM_ img_srcs $ findParttData (bldDir globalDirConfig</>"krnl"</>"kexec", "", out) (fromIntegral prtt_size)
    let initState = (0, (0, 0), (0, 0))
    (_, (krnl_loc, krnl_size), (usr_loc, usr_size)) <-
      liftIO $ execStateT f initState

    let blockSize = 512
    let defines = [
            ("krnl_loc_seg",  show (krnl_loc `div` blockSize)),
            ("krnl_loc_off",  show (krnl_loc `mod` blockSize)),
            ("krnl_size_seg", show (krnl_size `div` blockSize)),
            ("krnl_size_off", show (krnl_size `mod` blockSize)),

            ("usr_loc_seg",  show (usr_loc `div` blockSize)),
            ("usr_loc_off",  show (usr_loc `mod` blockSize)),
            ("usr_size_seg", show (usr_size `div` blockSize)),
            ("usr_size_off", show (usr_size `mod` blockSize))
          ]
    -- liftIO $ print defines

    nasmBuild (out, dep, src) prttDirConfig defaultNasmConfig{defines}

  distDir globalDirConfig</>"img" %> \out -> do
    need img_srcs
    () <- cmd Shell "cat" img_srcs ">" [out]
    return ()

  want [distDir globalDirConfig</>"img"]
