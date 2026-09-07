#include "cpu.hpp"
#include "bus.hpp"

Cpu::Cpu(Bus& bus) : bus(bus) { buildTable(); }

bool Cpu::getFlag(StatusFlag flag) const { return (status & flag) != 0; }

void Cpu::setFlag(StatusFlag flag, bool value) {
  if (value) {
    status |= flag;
  } else {
    status &= ~flag;
  }
}

/// Addressing mode, execution, cycles
void Cpu::buildTable() {
  table[0xA9] = {[this]() { return immediate(); },
                 [this](uint8_t v) { lda(v); }, 2};
  table[0xA5] = {[this]() { return zeroPage(); },
                 [this](uint8_t v) { lda(v); }, 3};
}

/// Addressing modes
uint8_t Cpu::immediate() {
  uint8_t value = bus.read(pc++);
  return value;
}

uint8_t Cpu::zeroPage() {
  uint8_t addr = bus.read(pc++);
  return bus.read(addr);
}



void Cpu::step() {
  uint8_t opcode = bus.read(pc++);
  Instruction& instruction = table[opcode];
  if (!instruction.execute) {

    return;
  }
  uint8_t operand = instruction.addressingMode();
  instruction.execute(operand);
}

void Cpu::lda(uint8_t value) {
  a = value;
  setFlag(Zero, a == 0);
  setFlag(Negative, (a & 0x80) != 0);
}
