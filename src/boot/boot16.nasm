%include "../prtt/def.nasm"

bits 16

org 0x7c00

krnl_final_addr: equ 0x0

stack_tmp: equ $-1
boot16:
  vid_mode:
    mov ah, 0 ; op N
    mov al, 3 ; 80x25
    int 0x10

  read_mem_listing:
    mov di, mem_listing ; dest
    mov bp, 0 ; non-0 = !first iteration

    xor ebx, ebx ; magic iteration num = 0 @ start
    mov edx, 'PAMS' ; magic num, SMAP backwards

    .loop:
    mov eax, 0xe820 ; op N
    ; ebx = magic iteration num
    mov ecx, 0xffff_ffff ; buffer size

    int 0x15
    jc .carry_set ; err/done

    ; cmp eax, 'PAMS'
    ; jne .fail ; err

    add di, cx ; cx = bytes read

    cmp cx, 24 ; check for eattr
    je .eattr_present
    mov dword [di], 1 ; non-volatile mem eattr
    add di, 4 ; sizeof dword
    .eattr_present:

    inc bp ; non-0 = !first iteration

    test ebx, ebx ; ebx = 0 => done
    je .done
    jmp .loop

    .carry_set:
    test bp, bp
    jnz .done ; carry + !first iteration => done
    .fail:
    jmp $ ; hlt

    .done:
    mov [pmem_listing_end], di

  read_prtt:
    mov bx, [pfree_mem] ; out loc

    mov al, prtt_size_sec ; sec count
    mov ch, 0 ; cyl #
    mov cl, 2 ; start sec # (1-based)
    mov dh, 0 ; head #
    mov dl, 0x80 ; drive # (0x80 = 'C')

    mov ah, 2 ; read
    int 0x13

  read_krnl:
    mov bx, [pfree_mem] ; out loc
    add bx, prtt_size

    mov si, [pprtt]

    movzx dx, byte [si + prtt.krnl + prtt_entry.loc_off]
    mov di, bx
    add di, dx
    mov [pkrnl_tmp], di

    ; al = sec count
    movzx ax, byte [si + prtt.krnl + prtt_entry.size_off]
    add ax, 511
    shr ax, 9
    add al, [si + prtt.krnl + prtt_entry.size_seg]
    ; ceil(size_off/512) + size_seg

    ; cl = start sec # (1-based)
    movzx cx, byte [si + prtt.krnl + prtt_entry.loc_off]
    add cx, 511
    shr cx, 9
    add cl, [si + prtt.krnl + prtt_entry.loc_seg]
    ; ceil(size_off/512) + size_seg

    mov ch, 0 ; cyl #
    mov dh, 0 ; head #
    mov dl, 0x80 ; drive # (0x80 = 'C')

    mov ah, 2 ; read
    int 0x13

  cli:
    cli

  a20:
    ; QEMU has a20 enabled by default
    ; fixme: implement actual a20 enabler

  lgdt:
    lgdt [gdtr_tmp]

  proto:
    smsw ax
    or al, 1
    lmsw ax

  jmp_boot32:
    ; fixme: may need explicit size specifier
    jmp gdt_tmp_code:boot32 ; ljmp to set code segment

%include "boot32.nasm"

; - - -

%include "gdt_entry.nasm"

gdt_tmp:
  dq 0 ; null-selector is invalid, no matter what the entry is, so set to an invalid value
gdt_tmp_code: equ $-gdt_tmp
  gdt_entry 0,0xffff_ffff,1,0,1,0,1
gdt_tmp_data: equ $-gdt_tmp
  gdt_entry 0,0xffff_ffff,1,0,0,0,1
gdtr_tmp:
  dw $-gdt_tmp-1
  dd gdt_tmp
; - - -

pmem_listing_end:
pfree_mem:
pprtt:
  dw 0
pkrnl_tmp:
  dw 0

boot_pad: times 512-2-($-$$) db 0
boot_sig: db 0x55,0xaa

boot_end:
mem_listing:
