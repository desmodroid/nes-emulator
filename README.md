# NES Emulator

A Nintendo Entertainment System emulator being built from scratch in C++, as a
learning project for understanding both emulation and the 6502 CPU at the
hardware level.

## Status

This is a work in progress. The CPU is the main focus so far; the PPU and APU
don't exist yet.

### CPU

- All addressing modes and all official instructions are implemented.
- Reset (hard and soft) is implemented, including the 7 startup cycles and the
  unused status flag.
- Total cycles are tracked, which is what the nestest log compares against.

TODO:
- interrupts (NMI/IRQ)
- unofficial/illegal opcodes
- validate against the full nestest log

### Bus

The bus routes CPU addresses:

| Range           | Target                                                |
|-----------------|-------------------------------------------------------|
| `$0000-$1FFF`   | 2KiB internal RAM, mirrored every `$0800`             |
| `$2000-$3FFF`   | PPU registers (stubbed, reads return 0)               |
| `$4000-$401F`   | APU / controller I/O (stubbed, reads return 0)        |
| `$4020-$FFFF`   | Cartridge                                             |

### Cartridge

iNES files are parsed (header, optional trainer, PRG ROM and CHR ROM). Mapper
support isn't implemented yet; PRG ROM is read directly from `$8000` up.

## Building

Requires CMake 3.20+ and a C++17 compiler. Catch2 is fetched automatically
via CMake's `FetchContent`.

```sh
cmake -B build
cmake --build build
```

## Running

```sh
./build/nes-emulator path/to/rom.nes
```

`main.cpp` loads the ROM, resets the CPU and steps it forever. It also has
commented-out code for running nestest: set the PC to `$C000` and print a
trace line per step, then diff against the reference `nestest.log`. ROMs and
logs are gitignored.

## Testing

```sh
./build/tests
```

Tests are written with Catch2. `tests/cpu_instructions_test.cpp` exercises the
CPU directly against a `Bus` with hand-assembled instruction bytes (via the
fixture in `tests/cpu_fixture.hpp`, which can set CPU state), and
`tests/cartridge_tests.cpp` covers iNES header parsing.

## Layout

```
include/    Public headers (Cpu, Bus, Cartridge)
src/        Implementation, split by concern:
              cpu.cpp                 - opcode table, step()
              cpu_addressing_modes.cpp
              cpu_instructions.cpp
              bus.cpp
              cartridge.cpp
tests/      Catch2 test suite
main.cpp    Entry point: loads a ROM and runs the CPU
```
