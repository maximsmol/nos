%ifndef hguard_kexec_def
%define hguard_kexec_def

struc kexec
  .maginum: resb 5 ; "kexec"
  .entry: resd 1
  .code: equ ($-%$strucname) ; hacks
endstruc

%endif
