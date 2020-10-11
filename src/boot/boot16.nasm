%include "../prtt/def.nasm"

%include "demo_macros.nasm"

bits 16

org 0x7c00

krnl_final_addr: equ 0x0

stack_tmp: equ $-1
boot16:
  mov sp, stack_tmp
  f_actively_cause_global_warming16 500

  f_demo_puts16 '[boot16]: set video mode'
  mov byte [demo_lines], 0

  vid_mode:
    mov ah, 0 ; op N
    mov al, 3 ; 80x25
    int 0x10

  f_demo_puts16 '[boot16]: read boot32'

  read_boot32:
    mov bx, boot32_section ; out loc

    mov al, 1 ; sec count
    mov ch, 0 ; cyl #
    mov cl, 2 ; start sec # (1-based)
    mov dh, 0 ; head #
    mov dl, 0x80 ; drive # (0x80 = 'C')

    mov ah, 2 ; read
    int 0x13

  f_demo_puts16 '[boot16]: read partition table'

  read_prtt:
    mov bx, pprtt ; out loc

    mov al, prtt_size_sec ; sec count
    mov ch, 0 ; cyl #
    mov cl, 3 ; start sec # (1-based)
    mov dh, 0 ; head #
    mov dl, 0x80 ; drive # (0x80 = 'C')

    mov ah, 2 ; read
    int 0x13

    mov ax, pprtt
    add ax, prtt_size
    mov [pfree_mem], ax

  f_demo_puts16_ptr boot16_kexec_message

  read_krnl:
    mov si, pprtt

    mov bx, [pfree_mem]
    mov dx, [si + prtt.krnl + prtt_entry.loc_off]
    add dx, bx
    mov [pkrnl_tmp], dx

    ; al = sec count (cannot be > 128)
    mov ax, [si + prtt.krnl + prtt_entry.size_off]
    add ax, 511
    shr ax, 9
    add ax, [si + prtt.krnl + prtt_entry.size_seg]
    ; ceil(size_off/512) + size_seg

    ; cl = start sec # (1-based)
    mov cx, [si + prtt.krnl + prtt_entry.loc_off]
    add cx, 511
    shr cx, 9
    add cx, [si + prtt.krnl + prtt_entry.loc_seg]
    ; ceil(loc_off/512) + loc_seg
    and cx, 0x1f ; have to mask this because bits 6+ are for the cylinder

    mov ch, 0 ; cyl #
    mov dh, 0 ; head #
    mov dl, 0x80 ; drive # (0x80 = 'C')

    push ax

    mov ah, 2 ; read
    int 0x13

    pop ax
    shl ax, 9
    mov bx, [pfree_mem]
    add bx, ax
    mov [pfree_mem], bx

  f_demo_puts16 '[boot16]: read mem map'

  read_mem_listing:
    mov di, [pfree_mem] ; dest
    mov [pmem_listing], di

    xor bp, bp ; non-0 = !first iteration

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
    mov [pfree_mem], di

  f_demo_puts16_ptr boot16_proto_message

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

%include "demo_tools16.nasm"

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

pfree_mem:
  dw 0
pkrnl_tmp:
  dw 0
pmem_listing:
  dw 0

boot_pad: times 512-2-($-$$) db 0
boot_sig: db 0x55,0xaa

%include "boot32.nasm"

%include "demo_tools32.nasm"

times 1024-($-$$) db 0

boot_end:
pprtt:
