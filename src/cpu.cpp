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

void Cpu::setZeroNegativeFlags(uint8_t value) {
  setFlag(Zero, value == 0);
  setFlag(Negative, (value & 0x80) != 0);
}

/// Addressing mode, execution, cycles
void Cpu::buildTable() {
  // clang-format off
  table[0xA9] = {[this]() { return immediate(); }, [this](uint16_t v) { lda(v); }, 2, false};
  table[0xA5] = {[this]() { return zeroPage();  }, [this](uint16_t v) { lda(v); }, 3, false};
  table[0xB5] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { lda(v); }, 4, false};
  table[0xAD] = {[this]() { return absolute();  }, [this](uint16_t v) { lda(v); }, 4, false};
  table[0xBD] = {[this]() { return absoluteX(); }, [this](uint16_t v) { lda(v); }, 4,  true};
  table[0xB9] = {[this]() { return absoluteY(); }, [this](uint16_t v) { lda(v); }, 4,  true};
  table[0xA1] = {[this]() { return indirectX(); }, [this](uint16_t v) { lda(v); }, 6, false};
  table[0xB1] = {[this]() { return indirectY(); }, [this](uint16_t v) { lda(v); }, 5,  true};

  table[0x85] = {[this]() { return zeroPage();  }, [this](uint16_t v) { sta(v); }, 3, false};
  table[0x95] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { sta(v); }, 4, false};
  table[0x8D] = {[this]() { return absolute();  }, [this](uint16_t v) { sta(v); }, 4, false};
  table[0x9D] = {[this]() { return absoluteX(); }, [this](uint16_t v) { sta(v); }, 5, false};
  table[0x99] = {[this]() { return absoluteY(); }, [this](uint16_t v) { sta(v); }, 5, false};
  table[0x81] = {[this]() { return indirectX(); }, [this](uint16_t v) { sta(v); }, 6, false};
  table[0x91] = {[this]() { return indirectY(); }, [this](uint16_t v) { sta(v); }, 6, false};

  table[0xA2] = {[this]() { return immediate(); }, [this](uint16_t v) { ldx(v); }, 2, false};
  table[0xA6] = {[this]() { return zeroPage();  }, [this](uint16_t v) { ldx(v); }, 3, false};
  table[0xB6] = {[this]() { return zeroPageY(); }, [this](uint16_t v) { ldx(v); }, 4, false};
  table[0xAE] = {[this]() { return absolute();  }, [this](uint16_t v) { ldx(v); }, 4, false};
  table[0xBE] = {[this]() { return absoluteY(); }, [this](uint16_t v) { ldx(v); }, 4,  true};

  table[0x86] = {[this]() { return zeroPage();  }, [this](uint16_t v) { stx(v); }, 3, false};
  table[0x96] = {[this]() { return zeroPageY(); }, [this](uint16_t v) { stx(v); }, 4, false};
  table[0x8E] = {[this]() { return absolute();  }, [this](uint16_t v) { stx(v); }, 4, false};

  table[0xA0] = {[this]() { return immediate(); }, [this](uint16_t v) { ldy(v); }, 2, false};
  table[0xA4] = {[this]() { return zeroPage();  }, [this](uint16_t v) { ldy(v); }, 3, false};
  table[0xB4] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { ldy(v); }, 4, false};
  table[0xAC] = {[this]() { return absolute();  }, [this](uint16_t v) { ldy(v); }, 4, false};
  table[0xBC] = {[this]() { return absoluteX(); }, [this](uint16_t v) { ldy(v); }, 4,  true};

  table[0x84] = {[this]() { return zeroPage();  }, [this](uint16_t v) { sty(v); }, 3, false};
  table[0x94] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { sty(v); }, 4, false};
  table[0x8C] = {[this]() { return absolute();  }, [this](uint16_t v) { sty(v); }, 4, false};

  table[0xAA] = {[this]() { return implied();   }, [this](uint16_t v) { tax(v); }, 2, false};
  table[0x8A] = {[this]() { return implied();   }, [this](uint16_t v) { txa(v); }, 2, false};
  table[0xA8] = {[this]() { return implied();   }, [this](uint16_t v) { tay(v); }, 2, false};
  table[0x98] = {[this]() { return implied();   }, [this](uint16_t v) { tya(v); }, 2, false};

  // clang-format on
}

void Cpu::step() {
  uint8_t opcode = bus.read(pc++);
  Instruction& instruction = table[opcode];
  if (!instruction.execute) {

    return;
  }
  auto [addr, pageCrossed] = instruction.addressingMode();
  instruction.execute(addr);
  uint8_t totalCycles =
      instruction.cycles + ((pageCrossed && instruction.canCrossPage) ? 1 : 0);
}
