.section .text
.global _start

_start:
    addi t0, x0, 1       # i
    addi t1, x0, 0       # sum
    addi t2, x0, 6       # stop at i == 6

loop:
    add  t1, t1, t0
    addi t0, t0, 1
    bne  t0, t2, loop

    # sum = 15 -> "15\n"
    la   t3, outbuf
    addi t4, x0, 49      # '1'
    sb   t4, 0(t3)
    addi t4, x0, 53      # '5'
    sb   t4, 1(t3)
    addi t4, x0, 10      # '\n'
    sb   t4, 2(t3)

    addi a0, x0, 1
    la   a1, outbuf
    addi a2, x0, 3
    addi a7, x0, 64
    ecall

    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
outbuf: .byte 0, 0, 0
