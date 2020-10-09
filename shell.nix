{ pkgs ? import <nixpkgs> {},
  i386pkgs ? (import <nixpkgs> {}).pkgsi686Linux }:

with pkgs;
let
  haskellDeps = ps: with ps; [
    generic-lens
    data-default
    lens
    shake
    Glob
  ];
  ghc = haskellPackages.ghcWithPackages haskellDeps;
in
pkgs.mkShell {
  buildInputs = (with pkgs; [
    ghc
    qemu
    nasm
    unixtools.xxd

    llvm
    lld
    clang-tools
    clang_10
    lldb_10

    bashInteractive
  ]);

  hardeningDisable = [ "fortify" ];
}
