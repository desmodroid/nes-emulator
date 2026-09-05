#include "cpu.hpp"
#include "bus.hpp"

Cpu::Cpu(Bus& bus) : bus(bus) {}

bool Cpu::getFlag(StatusFlag flag) const { return (status & flag) != 0; }

void Cpu::setFlag(StatusFlag flag, bool value) {
  if (value) {
    status |= flag;
  } else {
    status &= ~flag;
  }
}

void Cpu::lda(uint8_t value) {
  a = value;
  setFlag(Zero, a == 0);
  setFlag(Negative, (a & 0x80) != 0);   
}
