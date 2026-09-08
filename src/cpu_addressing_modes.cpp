#include "bus.hpp"
#include "cpu.hpp"

std::pair<uint16_t, bool> Cpu::immediate() {
  uint16_t addr = pc++;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::zeroPage() {
  uint8_t addr = bus.read(pc++);
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::zeroPageX() {
  uint8_t addr = bus.read(pc++);
  addr += x;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::zeroPageY() {
  uint8_t addr = bus.read(pc++);
  addr += y;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::absolute() {
  uint8_t low = bus.read(pc++);
  uint8_t high = bus.read(pc++);
  uint16_t addr = (high << 8) | low;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::absoluteX() {
  uint8_t low = bus.read(pc++);
  uint8_t high = bus.read(pc++);
  uint16_t base = (high << 8) | low;
  uint16_t addr = base + x;
  bool pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
  return {addr, pageCrossed};
}

std::pair<uint16_t, bool> Cpu::absoluteY() {
  uint8_t low = bus.read(pc++);
  uint8_t high = bus.read(pc++);
  uint16_t base = (high << 8) | low;
  uint16_t addr = base + y;
  bool pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
  return {addr, pageCrossed};
}

std::pair<uint16_t, bool> Cpu::indirectX() {
  uint8_t zpAddr = bus.read(pc++);
  zpAddr += x;
  uint8_t low = bus.read(zpAddr);
  uint8_t high = bus.read(static_cast<uint8_t>(zpAddr + 1));
  uint16_t addr = (high << 8) | low;
  return {addr, false};
}

std::pair<uint16_t, bool> Cpu::indirectY() {
  uint8_t zpAddr = bus.read(pc++);
  uint8_t low = bus.read(zpAddr);
  uint8_t high = bus.read(static_cast<uint8_t>(zpAddr + 1));
  uint16_t base = (high << 8) | low;
  uint16_t addr = base + y;
  bool pageCrossed = (base & 0xFF00) != (addr & 0xFF00);
  return {addr, pageCrossed};
}
