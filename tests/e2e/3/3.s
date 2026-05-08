.section .text
.global _start

_start:
    la   a1, msg
    addi a0, x0, 1
    addi a2, x0, 5
    addi a7, x0, 64
    ecall

    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
msg: .ascii "HELLO"
