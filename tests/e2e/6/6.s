.section .text
.global _start

_start:
    la   t0, cell
    addi t1, x0, 65      # 'A'
    sb   t1, 0(t0)
    lb   t2, 0(t0)

    sb   t2, 1(t0)

    addi a0, x0, 1
    addi a1, t0, 1
    addi a2, x0, 1
    addi a7, x0, 64
    ecall

    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
cell: .byte 0, 0
