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
                 [this](uint16_t v) { lda(v); }, 2};
  table[0xA5] = {[this]() { return zeroPage(); },
                 [this](uint16_t v) { lda(v); }, 3};
  table[0xB5] = {[this]() { return zeroPageX(); },
                 [this](uint16_t v) { lda(v); }, 4};
}

///  Addressing modes
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

void Cpu::step() {
  uint8_t opcode = bus.read(pc++);
  Instruction& instruction = table[opcode];
  if (!instruction.execute) {

    return;
  }
  uint16_t addr = instruction.addressingMode();
  instruction.execute(addr);
}

void Cpu::lda(uint16_t addr) {
  a = bus.read(addr);
  setFlag(Zero, a == 0);
  setFlag(Negative, (a & 0x80) != 0);
}
