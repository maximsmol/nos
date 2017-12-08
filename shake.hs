import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util

import Data.List
import System.Directory
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


shakeOptions' :: ShakeOptions
shakeOptions' = shakeOptions{
  shakeFiles = "bld"</>"shake"</>"db",
  shakeReport = (("bld"</>"rep") </>) <$> [
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

ensureDir :: String -> Rules ()
ensureDir = liftIO . createDirectoryIfMissing True

main :: IO ()
main = shakeArgs shakeOptions' $ do
  let path_in = "src"
  let path_out = "bld"
  let path_dep = path_out</>"dep"

  -- partition table paths
  let path_prttin = path_in</>"prtt"
  let path_prttout = path_out</>"prtt"
  let path_prttdep = path_dep</>"prtt"

  let path_prttbin = path_prttout</>"bin"
  let path_prttsize = path_prttout</>"size"

  -- boot paths
  let path_bin = path_in</>"boot"
  let path_bout = path_out</>"boot"
  let path_bdep = path_dep</>"boot"

  let path_bbin = path_bout</>"bin"

  let path_bobj = path_bout</>"obj"
  let path_bboot = path_bobj</>"boot"<.>"o"

  -- krnl paths
  let path_kin = path_in</>"krnl"
  let path_kout = path_out</>"krnl"
  let path_kdep = path_dep</>"krnl"

  let path_klink_script = path_kin</>"link"<.>"ld"

  let path_krnl = path_kout</>"krnl"<.>"o"
  let path_kbin = path_kout</>"bin"
  let path_kmain_addr = path_kout</>"kmain_addr"
  let path_kobj = path_kout</>"obj"
  let path_kkexec = path_kout</>"kexec"

  let ksrcs = ["main", "ata", "term"]

  -- dbg paths
  let path_dbg = path_out</>"dbg"

  let path_dmap = path_dbg</>"map"
  let path_dmap_boot = path_dmap</>"boot"
  let path_dmap_krnl = path_dmap</>"krnl"

  let path_ddisasm = path_dbg</>"disasm"
  let path_ddisasm_krnl = path_ddisasm</>"krnl"<.>"nasm"
  let path_ddisasm_boot = path_ddisasm</>"boot"<.>"nasm"

  ensureDir $ path_out</>"rep"

  ensureDir path_prttdep
  ensureDir path_bdep
  ensureDir path_kdep

  let img = "dst"</>"dbg"</>"img"
  want [img]

  let img_srcs = [path_bbin, path_prttbin, path_kkexec]
  img %> \out -> do
    need img_srcs
    need [path_kkexec]
    () <- cmd Shell "cat" img_srcs ">" [out]
    return ()

  -- partt
  [path_prttbin, path_prttdep</>(dropDir path_prttbin path_prttout)<.>"dep"] &%> \[out, dep] -> do
    let src = path_prttin</>"gen"<.>"nasm"
    need $ img_srcs \\ [path_prttbin]

    need [path_prttsize]
    prtt_sizef_raw <- liftIO $ BS.readFile path_prttsize
    let prtt_size = runGet getWord32le prtt_sizef_raw

    let f = forM_ img_srcs $ findParttData (path_kkexec, "", path_prttbin) (fromIntegral prtt_size)
    let initState = (0, (0, 0), (0, 0))
    (_, (krnl_loc, krnl_size), (usr_loc, usr_size)) <-
      liftIO $ execStateT f initState

    let blockSize = 512
    let defines = [
          "krnl_loc_seg=" ++show (krnl_loc `div` blockSize),
          "krnl_loc_off=" ++show (krnl_loc `mod` blockSize),
          "krnl_size_seg="++show (krnl_size `div` blockSize),
          "krnl_size_off="++show (krnl_size `mod` blockSize),

          "usr_loc_seg=" ++show (usr_loc `div` blockSize),
          "usr_loc_off=" ++show (usr_loc `mod` blockSize),
          "usr_size_seg="++show (usr_size `div` blockSize),
          "usr_size_off="++show (usr_size `mod` blockSize)
          ] >>= (("-D":).return)

    -- liftIO $ putStrLn (show defines)

    () <- cmd "nasm" "-O0" "-Wall" "-M" "-MF" [dep] "-i" [addTrailingPathSeparator path_prttin] defines "-o" [out] [src]
    needMakefileDependencies dep

    () <- cmd "nasm" "-O0" "-Wall" "-i" [addTrailingPathSeparator path_prttin] defines "-o" [out] [src]

    return ()

  [path_prttsize, path_prttdep</>(dropDir path_prttsize path_prttout)<.>"dep"] &%> \[out, dep] -> do
    let src = path_prttin</>(dropDir out path_prttout)<.>"nasm"

    () <- cmd "nasm" "-O0" "-Wall" "-M" "-MF" [dep] "-i" [addTrailingPathSeparator path_prttin] "-o" [out] [src]
    needMakefileDependencies dep

    () <- cmd "nasm" "-O0" "-Wall" "-i" [addTrailingPathSeparator path_prttin] "-o" [out] [src]
    return ()

  -- krnl
  path_kmain_addr %> \out -> do
    need [path_krnl]
    () <- cmd Shell "i686-elf-objdump" "-t" [path_krnl] "|" "grep kmain" "|" "ggrep -oP \"^[a-f0-9]+\"" "|" "tac" "-rs" ".." "|" "xxd" "-r" "-p" ">" [out]
    return ()

  path_krnl %> \out -> do
    let srcs = map (\x -> path_kobj</>x<.>"o") ksrcs
    need srcs
    () <- cmd "i686-elf-g++" "-Wl,-T" [path_klink_script] "-nostdlib" "-o" [out] srcs  "-lgcc"
    return ()

  path_kbin %> \out -> do
    need [path_krnl]
    let script = path_klink_script
    need [script]
    () <- cmd "i686-elf-ld" "-T" [path_klink_script] "--oformat" "binary" "-o" [out] [path_krnl]
    return ()

  path_kkexec %> \out -> do
    let srcs = [path_kmain_addr, path_kbin]
    need srcs
    () <- cmd Shell "printf" "kexec" ">" [out]
    () <- cmd Shell "cat" srcs ">>" [out]
    return ()

  -- boot
  path_bbin %> \out -> do
    need [path_bboot]
    cmd "cp" [path_bboot] [path_bbin]

  [path_bobj<//>"*"<.>"o", path_dmap_boot<//>"*", path_bdep<//>"*"<.>"dep"] &%> \[out, map, dep] -> do
    let src = path_bin</>(dropDir out path_bobj)-<.>"nasm"

    let command = ["nasm", "-O0", "-Wall", "-i", addTrailingPathSeparator path_bin, "-o", out]

    () <- cmd command "-M" "-MF" [dep] [src]
    needMakefileDependencies dep

    () <- cmd Shell command [src] ">" [map]
    return ()

  [path_kobj<//>"*"<.>"o", path_kdep<//>"*"<.>"dep"] &%> \[out, dep] -> do
    let src = path_kin</>(dropDir out path_kobj)-<.>"cpp"
    -- "-ffreestanding" "-fno-builtin" "-nostdinc" "-nostdlibinc" "-nobuiltininc"
    -- "-Ofast"
    -- "-Wno-c++98-compat" "-Wno-c++98-c++11-compat", "-Wno-c++98-c++11-compat-pedantic", "-Wno-c99-extensions", "-Wno-c++98-c++11-c++14-compat", "-Wno-padded", "-Wno-c++98-compat-pedantic"
    -- "-Winline"
    let warnFlags = ["-Wall", "-Wextra", "-pedantic"]
    let crossFlags = ["-ffreestanding"] --  "-mno-sse", "-nostdlib"
    let otherFlags = ["-std=c++1z", "-O0"]
    let command = ["i686-elf-g++", "-fdiagnostics-color=always"] ++ warnFlags ++ crossFlags ++ otherFlags ++ ["-o", out]

    () <- cmd command "-M" "-MF" [dep] [src]
    needMakefileDependencies dep

    () <- cmd command "-c" [src]
    return ()

  -- dbg
  phony "dbg" $ do
    let dbgFiles = [
          path_dmap_boot</>"boot",
          path_dmap_krnl,

          path_ddisasm_krnl,
          path_ddisasm_boot
          ]
    need dbgFiles

  path_dmap_krnl %> \out -> do
    let src = path_krnl
    need [src]
    () <- cmd Shell "i686-elf-objdump" "-t" [src] ">" [out]
    return ()

  path_ddisasm_krnl %> \out -> do
    let src = path_kbin
    need [src]
    () <- cmd Shell "ndisasm" "-b" "32" [src] ">" [out]
    return ()

  path_ddisasm_boot %> \out -> do
    let src = path_bbin
    need [src]
    () <- cmd Shell "ndisasm" "-b" "16" [src] ">" [out]
    return ()
