.section .text
.global _start

_start:
    jal  ra, make_q

    addi a0, x0, 1
    la   a1, out
    addi a2, x0, 1
    addi a7, x0, 64
    ecall

    addi a0, x0, 0
    addi a7, x0, 93
    ecall

make_q:
    la   t0, out
    addi t1, x0, 81      # 'Q'
    sb   t1, 0(t0)
    jalr x0, ra, 0

.section .data
out: .byte 0
