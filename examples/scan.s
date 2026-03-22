.section .data
buffer:     .space 16
out_buf:    .space 16
prompt:     .asciz "Enter: "

.section .text
.global _start

_start:
    # 1. Print Prompt
    li a7, 64          # sys_write
    li a0, 1           # stdout
    la a1, prompt
    li a2, 7
    ecall

    # 2. Read Input
    li a7, 63          # sys_read
    li a0, 0           # stdin
    la a1, buffer
    li a2, 16
    ecall

    # 3. ASCII to Integer (atoi) using Addition Loop
    mv t0, a0          # t0 = bytes read
    la t1, buffer
    li s1, 0           # s1 = cumulative result (n)
    li t3, 10          # multiplier

parse_loop:
    lb t4, 0(t1)
    li t5, 10          # newline check
    beq t4, t5, end_parse
    addi t4, t4, -48   # ASCII to digit
    bltz t4, end_parse

    # Multiplication: s1 = s1 * 10 (without 'mul')
    # Using: (s1 << 3) + (s1 << 1)  => 8x + 2x = 10x
    slli t5, s1, 3
    slli t6, s1, 1
    add s1, t5, t6
    add s1, s1, t4     # Add current digit

    addi t1, t1, 1
    j parse_loop

end_parse:
    # 4. Integer to ASCII (itoa) using Subtraction Loop
    la a1, out_buf
    addi a1, a1, 15    # Start at end of buffer
    li t0, 10          # newline
    sb t0, 0(a1)
    
    mv t4, s1          # t4 = value to convert
    li s2, 0           # digit counter

convert_loop:
    addi a1, a1, -1
    # Remainder and Division without 'rem'/'div'
    # We subtract 10 repeatedly to find the digit
    li t5, 0           # t5 = quotient
    mv t6, t4          # t6 = temporary remainder
find_digit:
    li t2, 10
    blt t6, t2, store_digit
    addi t6, t6, -10
    addi t5, t5, 1
    j find_digit

store_digit:
    addi t6, t6, 48    # Convert remainder to ASCII
    sb t6, 0(a1)
    mv t4, t5          # Update n with quotient
    addi s2, s2, 1
    bnez t4, convert_loop

    # 5. Write Result
    li a7, 64
    li a0, 1
    # a1 is already pointing to the start of our string
    addi a2, s2, 1     # length = digits + newline
    ecall

    # 6. Exit
    li a7, 93
    li a0, 0
    ecall
