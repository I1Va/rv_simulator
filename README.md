## RISC-V RV32I Simulator

This is a lightweight **RV32I (32-bit RISC-V Integer)** instruction set simulator. It is designed for educational purposes, featuring a QEMU-style execution log and support for pseudo-instructions (like `li`, `mv`, and `j`) to improve disassembly readability.

### Key Features

* **(almost)Complete RV32I Support:** Implements the standard base integer instruction set (without ebreak, ecall и fence.i).
* **Step-by-Step Execution:** Control exactly how many instructions to run using the `--s` flag.
* **Detailed State Dumps:** Prints the program counter, instruction hex, disassembly, and register state for every cycle.

---

### Getting Started

#### 1. Clone the Repository

```bash
git clone https://github.com/I1Va/rv_simulator.git
cd rv_simulator

```

#### 2. Build the Project

The simulator includes a build script to handle compilation via CMake.

```bash
chmod +x build.sh && ./build.sh
```

#### 3. Launch the Simulator

Run an ELF file by specifying the path. You can use the `--s` option to set the number of instructions executed

```bash
# Execute 10 instructions of the sum.elf example
./build/rv_simulator -s 10 examples/sum.elf
```

---

### Command Line Options

| Option | Description |
| --- | --- |
| `-s, --s <steps>` | Number of instructions to execute before stopping. |
| `--isa=<model>` | Set the ISA model (default: `rv32i`). |
| `--help, -h` | Show usage information. |

### Example Output

When running, the simulator provides a trace similar to the following:


```
Starting simulation: examples/sum.elf (ISA : rv32i, steps : 10)
----------------
IN: 
0x00010074:  00a00293      li                      t0,10

 pc       10074000
 x0/zero  00000000  x1/ra    00000000  x2/sp    00000000  x3/gp    00000000
 x4/tp    00000000  x5/t0    00000000  x6/t1    00000000  x7/t2    00000000
 x8/s0    00000000  x9/s1    00000000  x10/a0   00000000  x11/a1   00000000
 x12/a2   00000000  x13/a3   00000000  x14/a4   00000000  x15/a5   00000000
 x16/a6   00000000  x17/a7   00000000  x18/s2   00000000  x19/s3   00000000
 x20/s4   00000000  x21/s5   00000000  x22/s6   00000000  x23/s7   00000000
 x24/s8   00000000  x25/s9   00000000  x26/s10  00000000  x27/s11  00000000
 x28/t3   00000000  x29/t4   00000000  x30/t5   00000000  x31/t6   00000000
----------------
IN: 
0x00010078:  00000313      li                      t1,0

 pc       10078000
 x0/zero  00000000  x1/ra    00000000  x2/sp    00000000  x3/gp    00000000
 x4/tp    00000000  x5/t0    0000000a  x6/t1    00000000  x7/t2    00000000
 x8/s0    00000000  x9/s1    00000000  x10/a0   00000000  x11/a1   00000000
 x12/a2   00000000  x13/a3   00000000  x14/a4   00000000  x15/a5   00000000
 x16/a6   00000000  x17/a7   00000000  x18/s2   00000000  x19/s3   00000000
 x20/s4   00000000  x21/s5   00000000  x22/s6   00000000  x23/s7   00000000
 x24/s8   00000000  x25/s9   00000000  x26/s10  00000000  x27/s11  00000000
 x28/t3   00000000  x29/t4   00000000  x30/t5   00000000  x31/t6   00000000
----------------
....
```
