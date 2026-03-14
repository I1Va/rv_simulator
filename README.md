## RISC-V RV32I Simulator

This is a lightweight **RV32I (32-bit RISC-V Integer)** instruction set simulator. Designed for educational purposes, it features a QEMU-style execution log and supports pseudo-instructions (e.g., `li`, `mv`, `j`) to improve disassembly readability.

### Key Features

* **Comprehensive RV32I Support:** Implements the standard base integer instruction set (excluding `ebreak`, `ecall`, and `fence.i`).
* **Interactive REPL Mode:** Real-time debugging, register inspection, and memory manipulation.
* **Step-by-Step Execution:** Precisely control simulation length using the `-s` flag.
* **Detailed State Tracing:** Logs the program counter (PC), instruction hex, disassembly, and full register state for every cycle.

---

### Getting Started

#### 1. Clone the Repository

```bash
git clone https://github.com/I1Va/rv_simulator.git
cd rv_simulator
```

#### 2. Build the Project

The simulator uses CMake. For faster compilation, build in parallel:

```bash
cmake -S . -B build 
cmake --build build 
```

#### 3. Launch the Simulator

Run an ELF file by specifying its path.

```bash
# Execute 10 instructions of the sum.elf example
./build/rv_simulator -s 10 examples/sum.elf

# Start in interactive mode
./build/rv_simulator --interactive examples/sum.elf
```

#### 4. Run tests
``` bash
./build/tests/UnitTesting
```

---

### Command Line Options

| Option | Description |
| --- | --- |
| `-s, --steps <n>` | Number of instructions to execute before stopping. |
| `--interactive` | Enter interactive mode (REPL). |
| `-i, --init_state <path>` | Load initial PC and registers from a hex state file. |
| `-f, --final_state <path>` | Save machine state to a file after simulation finishes. |
| `--isa=<model>` | Set the ISA model (default: `rv32i`). |
| `--help, -h` | Show usage information. |

---

### Examples

The following examples use `examples/sum.elf`, generated from `examples/sum.s` by command:

```bash
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -o examples/sum.elf examples/sum.s
```

**Source Code (`sum.s`):**

```s
.section .text
.globl _start
_start:
    li x5, 10       # t0 = 10
    li x6, 0        # t1 = 0
    li x7, 1        # t2 = 1
loop:
    bgt x7, x5, end # if t2 > t0, break
    add x6, x6, x7  # t1 += t2
    addi x7, x7, 1  # t2++
    j loop          
end:
    li a7, 93       # exit syscall
    li a0, 0        
    ecall           

```

#### Interactive Mode

Launch with an initial state file and a target for the final state:

```bash
./build/rv_simulator --interactive --init_state examples/initial_state -f examples/final_state examples/sum.elf
```

```
✗ ./build/rv_simulator --interactive --init_state examples/initial_state -f examples/final_state examples/sum.elf
Initial state loaded from: examples/initial_state
Starting simulation: 
========================================
 Simulation Configuration:
----------------------------------------
  ISA Model    : rv32i
  ELF Path     : examples/sum.elf
  Init State   : examples/initial_state
  Step Limit   : 10
  Interactive  : 1
========================================
Interactive Mode: type 'h' for help
sim> h
Commands:
  s [n]       Step n instructions (default 1)
  r           Dump registers
  m <addr>    Dump memory at address
  p <addr>    Set PC to address
  q           Quit
sim> r
 pc       00010074
 x0/zero  00000000  x1/ra    00000001  x2/sp    00000002  x3/gp    00000003
 x4/tp    00000004  x5/t0    00000005  x6/t1    00000006  x7/t2    00000007
 x8/s0    00000008  x9/s1    00000009  x10/a0   0000000a  x11/a1   0000000b
 x12/a2   0000000c  x13/a3   0000000d  x14/a4   0000000e  x15/a5   0000000f
 x16/a6   000000f1  x17/a7   000000f2  x18/s2   000000f3  x19/s3   000000f4
 x20/s4   000000f5  x21/s5   000000f6  x22/s6   000000f7  x23/s7   000000f8
 x24/s8   000000f9  x25/s9   000000fa  x26/s10  000000fb  x27/s11  000000fc
 x28/t3   000000fd  x29/t4   000000fe  x30/t5   000000ff  x31/t6   00000ff1
sim> 
```

**Loading the Initial State:**
The simulator can load registers from a file. If a register file contains an `@` symbol at the first position, the PC will **not** be overwritten by the file. Since an ELF is loaded, the PC is automatically set to the `e_entry` address defined in the ELF header:

```cpp
// The PC is initialized from the ELF Entry Point
typedef struct {
    // ...
    Elf32_Addr e_entry; // Entry point virtual address
    // ...
} Elf32_Ehdr;

```

**Register Integrity:**
Note that while `initial_state` contains values for indices `0-31`, `x0` (zero) will always remain `00000000` regardless of the input file, per the RISC-V specification.


#### Step Mode

Execute a specific number of cycles:

```bash
./build/rv_simulator -s 3 --init_state examples/initial_state -f examples/final_state examples/sum.elf
```

Upon completion, the simulator saves the final state (PC followed by `x0-x31`) to the specified `final_state` file:

```text
0x00010080  # Final PC
0x00000000  # x0
0x00000001  # x1
...

```
