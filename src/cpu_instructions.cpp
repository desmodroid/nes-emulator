#include "bus.hpp"
#include "cpu.hpp"

// ============================================================
// Access Instructions
// ============================================================

void Cpu::lda(uint16_t addr) {
  a = bus.read(addr);
  updateZeroNegativeFlags(a);
}

void Cpu::sta(uint16_t addr) { bus.write(addr, a); }

void Cpu::ldx(uint16_t addr) {
  x = bus.read(addr);
  updateZeroNegativeFlags(x);
}

void Cpu::stx(uint16_t addr) { bus.write(addr, x); }

void Cpu::ldy(uint16_t addr) {
  y = bus.read(addr);
  updateZeroNegativeFlags(y);
}

void Cpu::sty(uint16_t addr) { bus.write(addr, y); }

// ============================================================
// Transfer Instructions
// ============================================================

void Cpu::tax(uint16_t /* unused */) {
  x = a;
  updateZeroNegativeFlags(x);
}

void Cpu::txa(uint16_t /* unused */) {
  a = x;
  updateZeroNegativeFlags(a);
}

void Cpu::tay(uint16_t /* unused */) {
  y = a;
  updateZeroNegativeFlags(y);
}

void Cpu::tya(uint16_t /* unused */) {
  a = y;
  updateZeroNegativeFlags(a);
}

// ============================================================
// Arithmetic Instructions
// ============================================================

void Cpu::adc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = a + memoryValue + carryBit;
  uint8_t result = static_cast<uint8_t>(sum);
  bool overflow = ((a ^ result) & (result ^ memoryValue) & 0x80) != 0;
  a = result;

  updateZeroNegativeFlags(a);
  setFlag(Carry, sum > 0xFF);
  setFlag(Overflow, overflow);
}

void Cpu::sbc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  memoryValue = ~memoryValue;
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = a + memoryValue + carryBit;
  uint8_t result = static_cast<uint8_t>(sum);
  bool overflow = ((a ^ result) & (result ^ memoryValue) & 0x80) != 0;
  a = result;

  updateZeroNegativeFlags(a);
  setFlag(Carry, sum > 0xFF);
  setFlag(Overflow, overflow);
}

void Cpu::inc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  memoryValue += 1;
  bus.write(addr, memoryValue);
  updateZeroNegativeFlags(memoryValue);
}

void Cpu::dec(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  memoryValue -= 1;
  bus.write(addr, memoryValue);
  updateZeroNegativeFlags(memoryValue);
}

void Cpu::inx(uint16_t /* unused */) {
  x += 1;
  updateZeroNegativeFlags(x);
}

void Cpu::dex(uint16_t /* unused */) {
  x -= 1;
  updateZeroNegativeFlags(x);
}

void Cpu::iny(uint16_t /* unused */) {
  y += 1;
  updateZeroNegativeFlags(y);
}

void Cpu::dey(uint16_t /* unused */) {
  y -= 1;
  updateZeroNegativeFlags(y);
}