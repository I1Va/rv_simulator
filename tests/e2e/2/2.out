.section .text
.global _start

_start:
    la   t0, msg
    addi t1, x0, 5       # Loop counter (5 chars)

loop:
    lb   a1, 0(t0)       # Load char
    
    # Write char
    addi a0, x0, 1
    addi a2, x0, 1
    addi a7, x0, 64
    ecall
    
    addi t0, t0, 1       # next char
    addi t1, t1, -1      # decrement
    bne  t1, x0, loop

    # Exit
    addi a0, x0, 0
    addi a7, x0, 93
    ecall

.section .data
msg: .ascii "RV32I"