bits 32

actively_cause_global_warming32:
  test ecx, ecx
  jz .done

  push ebx
  push eax

  xor al, al
  .warm_up:
    mov ebx, 0xffff
    .warm_up1:
      out 0x80, al
      dec ebx
      jnz .warm_up1
    dec ecx
    jnz .warm_up

  pop eax
  pop ebx

  .done:
  ret

vidmem_addr: dq 0xb8000
puts32:
  push ax
  push edi
  jmp .midloop

  .loop:
  mov edi, [vidmem_addr]
  mov [edi], al
  inc edi
  mov byte [edi], 7
  inc edi
  mov [vidmem_addr], edi
  inc esi
  .midloop:
  mov al, [esi]
  cmp al, 0
  jne .loop

  pop edi
  pop ax
  ret
