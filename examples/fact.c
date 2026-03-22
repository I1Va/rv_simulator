/* RISC-V Syscall Numbers */
#define SYS_READ  63
#define SYS_WRITE 64
#define SYS_EXIT  93

#define STDIN     0
#define STDOUT    1

/* Inline Assembly Wrapper for RISC-V ecall */
long syscall3(long num, long a0, long a1, long a2) {
    register long a7 __asm__("a7") = num;
    register long x10 __asm__("a0") = a0;
    register long x11 __asm__("a1") = a1;
    register long x12 __asm__("a2") = a2;
    __asm__ volatile (
        "ecall"
        : "+r"(x10)
        : "r"(a7), "r"(x11), "r"(x12)
        : "memory"
    );
    return x10;
}

unsigned long factorial(unsigned long n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}

void main() {
    unsigned long n = 5; // Example: 5!
    unsigned long result = factorial(n);

    char out[32];
    int pos = 30;
    out[pos--] = '\n';
    
    if (result == 0) {
        out[pos--] = '0';
    } else {
        while (result > 0 && pos >= 0) {
            out[pos--] = (result % 10) + '0';
            result /= 10;
        }
    }

    // Print result using your handle_write
    syscall3(SYS_WRITE, STDOUT, (long)&out[pos + 1], 30 - pos);

    // Exit using your handle_exit
    syscall3(SYS_EXIT, 0, 0, 0);
}