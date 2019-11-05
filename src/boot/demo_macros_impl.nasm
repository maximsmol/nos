%macro f_actively_cause_global_warming16 1
  push cx
  mov cx, %1
  call actively_cause_global_warming16
  pop cx
%endmacro

%macro f_actively_cause_global_warming32 1
  push ecx
  mov ecx, %1
  call actively_cause_global_warming32
  pop ecx
%endmacro

%macro f_puts16_ptr 1
  push si
  mov si, %1
  call puts16
  pop si
%endmacro

%macro f_puts16 1
  jmp %%skip
  %%data:
    db %1, `\n\r`, 0
  %%skip:
  f_puts16_ptr %%data
%endmacro

%macro f_puts32 1
  jmp %%skip
  %%data:
    db %1, 0
  %%skip:
  push esi
  mov esi, %%data
  call puts32
  pop esi
%endmacro

demopause: equ 130

%macro f_demo_puts16_ptr 1
  f_puts16_ptr %1
  f_actively_cause_global_warming16 demopause
%endmacro
%macro f_demo_puts16 1
  f_puts16 %1
  f_actively_cause_global_warming16 demopause
%endmacro

%macro f_demo_puts32 1
  f_puts32 %1
  f_actively_cause_global_warming32 demopause
%endmacro

%macro f_demo_nl32 0
  xor eax, eax
  xor ebx, ebx
  xor edx, edx
  mov ax, [vidmem_addr]
  mov bx, 80*2
  div bx
  mul bx
  add ax, 80*2
  sub ax, 16*2
  mov [vidmem_addr], ax
%endmacro
