%ifndef hguard_prtt_def
%define hguard_prtt_def

struc prtt_entry
  ; LBA + offset in 1st 512B segment

  .loc_seg: resq 1
  .loc_off: resb 1

  .size_seg: resq 1
  .size_off: resb 1
endstruc

struc prtt
  .maginum: resb 4 ; "prtt"
  .krnl: resb prtt_entry_size
  .usr : resb prtt_entry_size
endstruc

prtt_size_sec: equ (prtt_size + 511)/512 ; ceil(prtt_size/512)

%endif
