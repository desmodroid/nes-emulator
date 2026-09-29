#include "bus.hpp"
#include "cpu.hpp"

std::pair<uint16_t, bool> Cpu::immediate() {
  uint16_t addr = reg.pc++;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::zeroPage() {
  uint8_t addr = bus.read(reg.pc++);
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::zeroPageX() {
  uint8_t addr = bus.read(reg.pc++);
  addr += reg.x;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::zeroPageY() {
  uint8_t addr = bus.read(reg.pc++);
  addr += reg.y;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::absolute() {
  uint8_t low = bus.read(reg.pc++);
  uint8_t high = bus.read(reg.pc++);
  uint16_t addr = (high << 8) | low;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::absoluteX() {
  uint8_t low = bus.read(reg.pc++);
  uint8_t high = bus.read(reg.pc++);
  uint16_t base = (high << 8) | low;
  uint16_t addr = base + reg.x;
  bool pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
  return {addr, pageCrossed};
}

std::pair<uint16_t, bool> Cpu::absoluteY() {
  uint8_t low = bus.read(reg.pc++);
  uint8_t high = bus.read(reg.pc++);
  uint16_t base = (high << 8) | low;
  uint16_t addr = base + reg.y;
  bool pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
  return {addr, pageCrossed};
}

std::pair<uint16_t, bool> Cpu::indirectX() {
  uint8_t zpAddr = bus.read(reg.pc++);
  zpAddr += reg.x;
  uint8_t low = bus.read(zpAddr);
  uint8_t high = bus.read(static_cast<uint8_t>(zpAddr + 1));
  uint16_t addr = (high << 8) | low;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::indirectY() {
  uint8_t zpAddr = bus.read(reg.pc++);
  uint8_t low = bus.read(zpAddr);
  uint8_t high = bus.read(static_cast<uint8_t>(zpAddr + 1));
  uint16_t base = (high << 8) | low;
  uint16_t addr = base + reg.y;
  bool pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
  return {addr, pageCrossed};
}

std::pair<uint16_t, bool> Cpu::indirect() {
  uint8_t lowPtr = bus.read(reg.pc++);
  uint8_t highPtr = bus.read(reg.pc++);
  uint16_t ptr = (highPtr << 8) | lowPtr;

  uint8_t low = bus.read(ptr);
  uint16_t highAddr;

  if ((ptr & 0xFF) == 0xFF) {
    // low byte of ptr is 0xFF and so the CPU wraps around to the
    // start of the SAME page instead of crossing into the next one
    highAddr = ptr & 0xFF00;
  } else {
    highAddr = ptr + 1;
  }

  uint8_t high = bus.read(highAddr);
  uint16_t addr = (high << 8) | low;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::implied() { return {0, false}; }

std::pair<uint16_t, bool> Cpu::accumulator() { return {0, false}; }

std::pair<uint16_t, bool> Cpu::relative() {
  int8_t offset = static_cast<int8_t>(bus.read(reg.pc));
  reg.pc++;
  uint16_t addr = reg.pc + offset;
  return {addr, false};
}
