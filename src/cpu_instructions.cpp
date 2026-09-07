#include "bus.hpp"
#include "cpu.hpp"

void Cpu::lda(uint16_t addr) {
  a = bus.read(addr);
  setFlag(Zero, a == 0);
  setFlag(Negative, (a & 0x80) != 0);
}
