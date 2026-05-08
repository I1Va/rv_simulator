.section .text
.global _start

_start:
    addi t0, x0, 2
    addi t1, x0, 3
    add  t2, t0, t1
    addi t2, t2, 48      # '0' + 5

    la   t3, outbuf
    sb   t2, 0(t3)
    addi t4, x0, 10      # '\n'
    sb   t4, 1(t3)

    addi a0, x0, 1
    la   a1, outbuf
    addi a2, x0, 2
    addi a7, x0, 64
    ecall

    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
outbuf: .byte 0, 0
