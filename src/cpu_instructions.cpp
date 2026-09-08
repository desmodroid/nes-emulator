#include "bus.hpp"
#include "cpu.hpp"

void Cpu::lda(uint16_t addr) {
  a = bus.read(addr);
  setFlag(Zero, a == 0);
  setFlag(Negative, (a & 0x80) != 0);
}

void Cpu::ldx(uint16_t addr) {
  x = bus.read(addr);
  setFlag(Zero, x == 0);
  setFlag(Negative, (x & 0x80) != 0);
}

void Cpu::ldy(uint16_t addr) {
  y = bus.read(addr);
  setFlag(Zero, y == 0);
  setFlag(Negative, (y & 0x80) != 0);
}
