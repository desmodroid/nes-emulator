#include "bus.hpp"
#include "cpu.hpp"

uint16_t Cpu::immediate() {
  uint16_t addr = pc++;
  return addr;
}

uint16_t Cpu::zeroPage() {
  uint8_t addr = bus.read(pc++);
  return addr;
}

uint16_t Cpu::zeroPageX() {
  uint8_t addr = bus.read(pc++);
  addr += x;
  return addr;
}
