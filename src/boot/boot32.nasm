bits 32

%include "../prtt/def.nasm"
%include "../kexec/def.nasm"

boot32:
  mov ax, gdt_tmp_data ; set segments
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov fs, ax
  mov gs, ax
  mov esp, stack_tmp

  mov_kernel:
    mov esi, [pkrnl_tmp]
    add esi, kexec.code

    mov edi, krnl_final_addr

    ; mov eax, word [pprtt]
    movzx eax, word [pprtt]

    ; ecx = size_seg*512
    mov ecx, [eax + prtt.krnl + prtt_entry.size_seg]
    shl ecx, 9

    movzx ebx, byte [eax + prtt.krnl + prtt_entry.size_off]
    add ecx, ebx

    ; ecx = ceil((size_seg*512 + size_off)/4)
    add ecx, 1
    mov ebx, ecx
    shr ecx, 2 ; log2(sizeof dword)

    rep movsd

  ; kmain(*mem_listing_start, *mem_listing_end, *krnl_end)
  movzx eax, word [pmem_listing_end]
  push eax

  push mem_listing
  push ebx

  mov eax, [pkrnl_tmp]
  add eax, kexec.entry
  mov eax, [eax]
  add eax, krnl_final_addr ; fixme: mov eax, [a + b] = ?
  call eax

  jmp $ ; hlt if kmain returns
