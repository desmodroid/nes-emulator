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
