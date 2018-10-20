%include "def.nasm"

istruc prtt
  at .maginum, db "prtt"
  at .krnl
    istruc prtt_entry
      at prtt_entry.loc_seg,  dq krnl_loc_seg
      at prtt_entry.loc_off,  dd krnl_loc_off

      at prtt_entry.size_seg, dq krnl_size_seg
      at prtt_entry.size_off, dd krnl_size_off
    iend
  at .usr
    istruc prtt_entry
      at prtt_entry.loc_seg,  dq usr_loc_seg
      at prtt_entry.loc_off,  dd usr_loc_off

      at prtt_entry.size_seg, dq usr_size_seg
      at prtt_entry.size_off, dd usr_size_off
    iend
iend
