.section .text
.globl _start

_start:
    li x5, 10       # x5 = 10 (Our 'n' value)
    li x6, 0        # x6 = 0  (Our accumulator/sum)
    li x7, 1        # x7 = 1  (Our counter 'i')

loop:
    bgt x7, x5, end # if i > n, jump to end
    add x6, x6, x7  # sum = sum + i
    addi x7, x7, 1  # i = i + 1
    j loop          # jump back to loop

end:
    nop          
    