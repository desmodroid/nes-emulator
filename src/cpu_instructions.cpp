#include "bus.hpp"
#include "cpu.hpp"

// ============================================================
// Access Instructions
// ============================================================

void Cpu::lda(uint16_t addr) {
  a = bus.read(addr);
  setZeroNegativeFlags(a);
}

void Cpu::sta(uint16_t addr) { bus.write(addr, a); }

void Cpu::ldx(uint16_t addr) {
  x = bus.read(addr);
  setZeroNegativeFlags(x);
}

void Cpu::stx(uint16_t addr) { bus.write(addr, x); }

void Cpu::ldy(uint16_t addr) {
  y = bus.read(addr);
  setZeroNegativeFlags(y);
}

void Cpu::sty(uint16_t addr) { bus.write(addr, y); }

// ============================================================
// Transfer Instructions
// ============================================================

void Cpu::tax(uint16_t /* unused */) {
  x = a;
  setZeroNegativeFlags(x);
}

void Cpu::txa(uint16_t /* unused */) {
  a = x;
  setZeroNegativeFlags(a);
}

void Cpu::tay(uint16_t /* unused */) {
  y = a;
  setZeroNegativeFlags(y);
}

void Cpu::tya(uint16_t /* unused */) {
  a = y;
  setZeroNegativeFlags(a);
}

// ============================================================
// Transfer Instructions
// ============================================================

// ADC adds the carry flag and a memory value to the accumulator. The carry flag
// is then set to the carry value coming out of bit 7, allowing values larger
// than 1 byte to be added together by carrying the 1 into the next byte's
// addition. This can also be thought of as unsigned overflow. It is common to
// clear carry with CLC before adding the first byte to ensure it is in a known
// state, avoiding an off-by-one error. The overflow flag indicates whether
// signed overflow or underflow occurred. This happens if both inputs are
// positive and the result is negative, or both are negative and the result is
// positive.

void Cpu::adc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = a + memoryValue + carryBit;
  uint8_t result = static_cast<uint16_t>(sum);

  setZeroNegativeFlags(a);
  setFlag(Carry, (sum > 0xFF) == 1);
  setFlag(Overflow, ((a ^ result) & (result ^ addr) & 0x80) != 0);
  a = result;
}