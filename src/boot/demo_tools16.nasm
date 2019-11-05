bits 16

actively_cause_global_warming16:
  test cx, cx
  jz .done

  push bx
  push ax

  xor al, al
  .warm_up:
    mov bx, 0xffff
    .warm_up1:
      out 0x80, al
      dec bx
      jnz .warm_up1
    dec cx
    jnz .warm_up

  pop ax
  pop bx

  .done:
  ret

demo_lines:
  db 0
puts16:
  push ax
  push bx
  mov ah, 0xe
  mov bx, 0
  jmp .midloop

  .loop:
  cmp al, `\n`
  jne .print
  inc byte [demo_lines]

  .print:
  int 0x10
  inc si
  .midloop:
  mov al, [si]
  cmp al, 0
  jne .loop

  pop bx
  pop ax
  ret
