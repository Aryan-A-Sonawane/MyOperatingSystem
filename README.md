# MyOperatingSystem
# A Virtual Machine Simulator in C

This project simulates a basic **virtual machine** written in C that mimics the behavior of a simplified operating system and CPU. It reads a batch of "job programs" from `input.txt`, processes them, and produces output in `output.txt`.

---

## 🧠 Project Overview

- **Memory**: 400 bytes divided into 100 words (4 bytes each)
- **Registers**:
  - `R` (General Purpose Register - 4 bytes)
  - `IR` (Instruction Register - 4 bytes): holds current instruction `{OPCODE, OPERAND}`
  - `IC` (Instruction Counter): points to the next instruction
  - `C` (Flag Register): comparison flag (0/1)
  - `SI` (System Interrupt): {0, 1, 2, 3}

---

## 💾 Instruction Set

| Instruction | Description |
|-------------|-------------|
| `LRxx` | Load word from memory[xx] into Register `R` |
| `SRxx` | Store content of `R` into memory[xx] |
| `CPxx` | Compare content of `R` with memory[xx], set `C` flag if equal |
| `BTxx` | Branch to memory[xx] if `C` flag is set |
| `GDxx` | Read 40 bytes from the Data Section (`$D`) into memory[xx] onward |
| `PDxx` | Print 40 bytes from memory[xx] onward to `output.txt` |
| `H`    | Halt the current program |

> `GD`, `PD`, and `H` are **Privileged (Kernel-only)** instructions  
> `LR`, `SR`, `CP`, `BT` are **User-level** instructions  

---

## 📁 File I/O Details

- **input.txt**: Program Card (input source)
- **output.txt**: Line Printer (output destination)

---

## 🏗️ Structure of a Job

Jobs in `input.txt` must be enclosed within **job control cards**:

- `$A` – Start of job (Allocates memory)
- `$D` – Start of data (Input for GDxx)
- `$E` – End of job

---

## 🧪 Writing a Sample Job

Here’s an example of a simple job in `input.txt` that:
1. Loads a number from memory[10]
2. Compares it to value at memory[20]
3. If equal, prints data from memory[30]
4. Halts execution

```txt
$A
GD10
GD20
GD30
LR10
CP20
BT06
PD30
H
$D
1234
1234
DATA TO BE PRINTED ONLY IF MATCHES
$E
