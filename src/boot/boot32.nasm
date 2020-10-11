bits 32

%include "../prtt/def.nasm"
%include "../kexec/def.nasm"

boot32_section:

boot16_kexec_message: db '[boot16]: read kernel kexec', `\n\r`, 0
boot16_proto_message: db '[boot16]: enter protected mode', `\n\r`, 0

boot32:
  mov ax, gdt_tmp_data ; set segments
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov fs, ax
  mov gs, ax
  mov esp, stack_tmp

  xor eax, eax
  xor ebx, ebx
  mov bl, 80*2
  mov al, [demo_lines]
  mul bl
  add [vidmem_addr], eax

  f_demo_puts32 '[boot32]: relocate kernel'
  f_demo_nl32

  xor eax, eax
  xor ebx, ebx
  xor edx, edx

  mov_kernel:
    movzx esi, word [pkrnl_tmp]
    add esi, kexec.code

    mov edi, krnl_final_addr

    ; mov eax, word [pprtt]
    mov eax, pprtt

    ; ecx = size_seg*512
    mov ecx, [eax + prtt.krnl + prtt_entry.size_seg]
    shl ecx, 9

    movzx ebx, word [eax + prtt.krnl + prtt_entry.size_off]
    add ecx, ebx

    ; ecx = ceil((size_seg*512 + size_off)/4) ; 4 = sizeof dword
    add ecx, 3 ; sizeof dword - 1
    mov ebx, ecx
    shr ecx, 2 ; log2(sizeof dword)

    rep movsd

  f_demo_puts32 '[boot32]: call kmain'

  ; kmain(*mem_listing_start, *mem_listing_end, *krnl_end)
  push edi

  movzx eax, word [pfree_mem]
  push eax

  movzx eax, word [pmem_listing]
  push eax

  movzx eax, word [pkrnl_tmp]
  add eax, kexec.entry
  mov eax, [eax]
  add eax, krnl_final_addr ; fixme: mov eax, [a + b] = ?

  call eax

  jmp $ ; hlt if kmain returns
