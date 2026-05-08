.section .text
.global _start

_start:
    addi t0, x0, 10      # t0 = 10
    slli t0, t0, 2       # t0 = 40 (10 << 2)
    addi t0, t0, 2       # t0 = 42 ('*' in ASCII)
    
    # Store to memory to print
    la   a1, res
    sb   t0, 0(a1)
    
    # Write syscall
    addi a0, x0, 1       # stdout
    addi a2, x0, 1       # length
    addi a7, x0, 64      # sys_write
    ecall

    # Exit
    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
res: .byte 0