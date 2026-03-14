.section .text
.globl _start

_start:
    li x5, 10       
    li x6, 0        
    li x7, 1        

loop:
    bgt x7, x5, end 
    add x6, x6, x7  
    addi x7, x7, 1  
    j loop          

end:
    li a7, 93       
    li a0, 0        
    ecall           
    
    