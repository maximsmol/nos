%include "def.nasm"

istruc partt
  at .maginum, db "kexec"
  at .entry, db kexec_entry
  at .code ; put nothing
iend
