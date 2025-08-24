.intel_syntax noprefix
.section .data
msg:
    .string "Hello, World!\n"
len = . - msg

.section .text
.global _start

_start:
    mov rax, 1          # syscall: write
    mov rdi, 1          # fd = stdout
    lea rsi, [rip+msg]  # address of msg
    mov rdx, len        # length
    syscall

    mov rax, 60         # syscall: exit
    xor rdi, rdi        # code 0
    syscall
