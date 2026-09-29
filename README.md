# NES Emulator

A Nintendo Entertainment System emulator being built from scratch in C++, as a
learning project for understanding both emulation and the 6502 CPU at the
hardware level.

## Status

This is a work in progress. Right now the focus is entirely on the CPU
(`Cpu`) — the PPU, APU, and cartridge/mapper support don't exist yet, and
`Bus` is currently just a flat 64KiB memory array rather than a real
memory-mapped router.

### CPU

All addressing modes are implemented.

All official instructions are implemented.

TODO: 
- CPU `reset()`
- interrupts
- unofficial/illegal opcodes

## Building

Requires CMake 3.20+ and a C++17 compiler. Catch2 is fetched automatically
via CMake's `FetchContent`.

```sh
cmake -B build
cmake --build build
```

## Testing

```sh
./build/tests
```

Tests are written with Catch2 and live in `tests/cpu_instructions_test.cpp`,
exercising the CPU directly against a `Bus` with hand-assembled instruction
bytes.

## Layout

```
include/    Public headers (Cpu, Bus)
src/        Implementation, split by concern:
              cpu.cpp                 - opcode table, step()
              cpu_addressing_modes.cpp
              cpu_instructions.cpp
              bus.cpp
tests/      Catch2 test suite
main.cpp    Entry point (currently a stub)
```
