%include "def.nasm"

istruc prtt
  at .maginum, db "prtt"
  at .krnl
    istruc prtt_entry
      at prtt_entry.loc_seg,  dq krnl_loc_seg
      at prtt_entry.loc_off,  db krnl_loc_off

      at prtt_entry.size_seg, dq krnl_size_seg
      at prtt_entry.size_off, db krnl_size_off
    iend
  at .usr
    istruc prtt_entry
      at prtt_entry.loc_seg,  dq usr_loc_seg
      at prtt_entry.loc_off,  db usr_loc_off

      at prtt_entry.size_seg, dq usr_size_seg
      at prtt_entry.size_off, db usr_size_off
    iend
iend
