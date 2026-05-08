.section .text
.global _start

_start:
    addi t0, x0, 1
    slli t0, t0, 6       # 64
    addi t1, x0, 26
    or   t2, t0, t1      # 90 = 'Z'

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
