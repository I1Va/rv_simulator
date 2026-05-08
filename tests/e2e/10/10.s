.section .text
.global _start

_start:
    addi t0, x0, -1      # 0xFFFFFFFF
    addi t1, x0, 1
    bltu t0, t1, less_unsigned

    addi t2, x0, 85      # 'U'
    jal  x0, store

less_unsigned:
    addi t2, x0, 76      # 'L'

store:
    la   t3, out
    sb   t2, 0(t3)

    addi a0, x0, 1
    la   a1, out
    addi a2, x0, 1
    addi a7, x0, 64
    ecall

    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
out: .byte 0
