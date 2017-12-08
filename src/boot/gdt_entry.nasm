%ifndef hguard_gdt_entry
%define hguard_gdt_entry

; base limit accessed=0 r/w conforming/direction exec ring present=1 size granularity=auto
; r (if code) - readable; never writable
; w (if data) - writable; always readable
; conforming (if code) - allow jump from lower ring
; direction (if data) - spans down (1)/up (0)
; exec - data (0)/code (1)
; size - 16-bit (0)/32-bit (1)
; granularity - limit in B (0)/4 KiB (1) blocks
%macro gdt_entry 7
  %if (%2 > 1111_1111_1111_1111_1111) && ((%2 & 4*1024-1) != 4*1024-1)
    %error "limit cannot be encoded"
  %endif

  %if (%2 > 1111_1111_1111_1111_1111) ; encode limit in 4KiBs blocks
    db ((%2 / (4*1024) >> 0) & 0xff)
    db ((%2 / (4*1024) >> 8) & 0xff)
  %else
    db ((%2 >> 0) & 0xff)
    db ((%2 >> 8) & 0xff)
  %endif

  db ((%1 >> 0 ) & 0xff)
  db ((%1 >> 8 ) & 0xff)
  db ((%1 >> 16) & 0xff)

  ; present=1 ring 1 exec conforming/direction r/w accessed=0
  db ((1 << 7) + ((%6 & 11b) << 5) + (1 << 4) + ((%5 & 1b) << 3) + ((%4 & 1b) << 2) + ((%3 & 1b) << 1) + 0)
  %if (%2 > 1111_1111_1111_1111_1111) ; encode limit in 4KiBs blocks
    db ((1 << 7) + (%7 << 6) + (0 << 5) + (0 << 4) + ((%2 / (4*1024) >> 16) & 0xf))
  %else
    db ((0 << 7) + (%7 << 6) + (0 << 5) + (0 << 4) + ((%2 >> 16) & 0xf))
  %endif

  db ((%1 >> 24) & 0xff)
%endmacro

%endif
