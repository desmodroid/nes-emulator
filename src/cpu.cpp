#include "cpu.hpp"
#include "bus.hpp"

Cpu::Cpu(Bus& bus) : bus(bus) { buildTable(); }

/// Addressing mode, execution, cycles
void Cpu::buildTable() {
  // clang-format off

  // -------- Access Instructions --------
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

  // -------- Transfer Instructions --------
  table[0xAA] = {[this]() { return implied();   }, [this](uint16_t v) { tax(v); }, 2, false};
  table[0x8A] = {[this]() { return implied();   }, [this](uint16_t v) { txa(v); }, 2, false};
  table[0xA8] = {[this]() { return implied();   }, [this](uint16_t v) { tay(v); }, 2, false};
  table[0x98] = {[this]() { return implied();   }, [this](uint16_t v) { tya(v); }, 2, false};

  // -------- Arithmetic Instructions --------
  table[0x69] = {[this]() { return immediate(); }, [this](uint16_t v) { adc(v); }, 2, false};
  table[0x65] = {[this]() { return zeroPage();  }, [this](uint16_t v) { adc(v); }, 3, false};
  table[0x75] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { adc(v); }, 4, false};
  table[0x6D] = {[this]() { return absolute();  }, [this](uint16_t v) { adc(v); }, 4, false};
  table[0x7D] = {[this]() { return absoluteX(); }, [this](uint16_t v) { adc(v); }, 4, false};
  table[0x79] = {[this]() { return absoluteY(); }, [this](uint16_t v) { adc(v); }, 4, false};
  table[0x61] = {[this]() { return indirectX(); }, [this](uint16_t v) { adc(v); }, 6, false};
  table[0x71] = {[this]() { return indirectY(); }, [this](uint16_t v) { adc(v); }, 5, false};

  table[0xE9] = {[this]() { return immediate(); }, [this](uint16_t v) { sbc(v); }, 2, false};
  table[0xE5] = {[this]() { return zeroPage();  }, [this](uint16_t v) { sbc(v); }, 3, false};
  table[0xF5] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { sbc(v); }, 4, false};
  table[0xED] = {[this]() { return absolute();  }, [this](uint16_t v) { sbc(v); }, 4, false};
  table[0xFD] = {[this]() { return absoluteX(); }, [this](uint16_t v) { sbc(v); }, 4, false};
  table[0xF9] = {[this]() { return absoluteY(); }, [this](uint16_t v) { sbc(v); }, 4, false};
  table[0xE1] = {[this]() { return indirectX(); }, [this](uint16_t v) { sbc(v); }, 6, false};
  table[0xF1] = {[this]() { return indirectY(); }, [this](uint16_t v) { sbc(v); }, 5, false};

  table[0xE6] = {[this]() { return zeroPage();  }, [this](uint16_t v) { inc(v); }, 5, false};
  table[0xF6] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { inc(v); }, 6, false};
  table[0xEE] = {[this]() { return absolute();  }, [this](uint16_t v) { inc(v); }, 6, false};
  table[0xFE] = {[this]() { return absoluteX(); }, [this](uint16_t v) { inc(v); }, 7, false};

  table[0xC6] = {[this]() { return zeroPage();  }, [this](uint16_t v) { dec(v); }, 5, false};
  table[0xD6] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { dec(v); }, 6, false};
  table[0xCE] = {[this]() { return absolute();  }, [this](uint16_t v) { dec(v); }, 6, false};
  table[0xDE] = {[this]() { return absoluteX(); }, [this](uint16_t v) { dec(v); }, 7, false};

  table[0xE8] = {[this]() { return implied();   }, [this](uint16_t v) { inx(v); }, 2, false};
  table[0xCA] = {[this]() { return implied();   }, [this](uint16_t v) { dex(v); }, 2, false};
  table[0xC8] = {[this]() { return implied();   }, [this](uint16_t v) { iny(v); }, 2, false};
  table[0x88] = {[this]() { return implied();   }, [this](uint16_t v) { dey(v); }, 2, false};

  // -------- Shift Instructions --------
  table[0x0A] = {[this]() { return accumulator();  }, [this](uint16_t v) { asl_a(v);}, 2, false};
  table[0x06] = {[this]() { return zeroPage();  }, [this](uint16_t v) { asl(v); }, 5, false};
  table[0x16] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { asl(v); }, 6, false};
  table[0x0E] = {[this]() { return absolute();  }, [this](uint16_t v) { asl(v); }, 6, false};
  table[0x1E] = {[this]() { return absoluteX(); }, [this](uint16_t v) { asl(v); }, 7, false};

  table[0x4A] = {[this]() { return accumulator();  }, [this](uint16_t v) { lsr_a(v);}, 2, false};
  table[0x46] = {[this]() { return zeroPage();  }, [this](uint16_t v) { lsr(v); }, 5, false};
  table[0x56] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { lsr(v); }, 6, false};
  table[0x4E] = {[this]() { return absolute();  }, [this](uint16_t v) { lsr(v); }, 6, false};
  table[0x5E] = {[this]() { return absoluteX(); }, [this](uint16_t v) { lsr(v); }, 7, false};

  table[0x2A] = {[this]() { return accumulator(); }, [this](uint16_t v) { rol_a(v); }, 2, false};
  table[0x26] = {[this]() { return zeroPage();  }, [this](uint16_t v) { rol(v); }, 5, false};
  table[0x36] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { rol(v); }, 6, false};
  table[0x2E] = {[this]() { return absolute();  }, [this](uint16_t v) { rol(v); }, 6, false};
  table[0x3E] = {[this]() { return absoluteX(); }, [this](uint16_t v) { rol(v); }, 7, false};

  table[0x6A] = {[this]() { return accumulator(); }, [this](uint16_t v) { ror_a(v); }, 2, false};
  table[0x66] = {[this]() { return zeroPage();  }, [this](uint16_t v) { ror(v); }, 5, false};
  table[0x76] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { ror(v); }, 6, false};
  table[0x6E] = {[this]() { return absolute();  }, [this](uint16_t v) { ror(v); }, 6, false};
  table[0x7E] = {[this]() { return absoluteX(); }, [this](uint16_t v) { ror(v); }, 7, false};

  // -------- Bitwise Instructions --------
  table[0x29] = {[this]() { return immediate(); }, [this](uint16_t v) { and_a(v); }, 2, false};
  table[0x25] = {[this]() { return zeroPage();  }, [this](uint16_t v) { and_a(v); }, 3, false};
  table[0x35] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { and_a(v); }, 4, false};
  table[0x2D] = {[this]() { return absolute();  }, [this](uint16_t v) { and_a(v); }, 4, false};
  table[0x3D] = {[this]() { return absoluteX(); }, [this](uint16_t v) { and_a(v); }, 4,  true};
  table[0x39] = {[this]() { return absoluteY(); }, [this](uint16_t v) { and_a(v); }, 4,  true};
  table[0x21] = {[this]() { return indirectX(); }, [this](uint16_t v) { and_a(v); }, 6, false};
  table[0x31] = {[this]() { return indirectY(); }, [this](uint16_t v) { and_a(v); }, 5,  true};

  table[0x09] = {[this]() { return immediate(); }, [this](uint16_t v) { ora(v); }, 2, false};
  table[0x05] = {[this]() { return zeroPage();  }, [this](uint16_t v) { ora(v); }, 3, false};
  table[0x15] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { ora(v); }, 4, false};
  table[0x0D] = {[this]() { return absolute();  }, [this](uint16_t v) { ora(v); }, 4, false};
  table[0x1D] = {[this]() { return absoluteX(); }, [this](uint16_t v) { ora(v); }, 4,  true};
  table[0x19] = {[this]() { return absoluteY(); }, [this](uint16_t v) { ora(v); }, 4,  true};
  table[0x01] = {[this]() { return indirectX(); }, [this](uint16_t v) { ora(v); }, 6, false};
  table[0x11] = {[this]() { return indirectY(); }, [this](uint16_t v) { ora(v); }, 5,  true};

  table[0x49] = {[this]() { return immediate(); }, [this](uint16_t v) { eor(v); }, 2, false};
  table[0x45] = {[this]() { return zeroPage();  }, [this](uint16_t v) { eor(v); }, 3, false};
  table[0x55] = {[this]() { return zeroPageX(); }, [this](uint16_t v) { eor(v); }, 4, false};
  table[0x4D] = {[this]() { return absolute();  }, [this](uint16_t v) { eor(v); }, 4, false};
  table[0x5D] = {[this]() { return absoluteX(); }, [this](uint16_t v) { eor(v); }, 4,  true};
  table[0x59] = {[this]() { return absoluteY(); }, [this](uint16_t v) { eor(v); }, 4,  true};
  table[0x41] = {[this]() { return indirectX(); }, [this](uint16_t v) { eor(v); }, 6, false};
  table[0x51] = {[this]() { return indirectY(); }, [this](uint16_t v) { eor(v); }, 5,  true};

  table[0x24] = {[this]() { return zeroPage();  }, [this](uint16_t v) { bit(v); }, 3, false};
  table[0x2c] = {[this]() { return absolute();  }, [this](uint16_t v) { bit(v); }, 4, false};

  // -------- Compare Instructions --------
  table[0xC9] = {[this]() { return immediate();  }, [this](uint16_t v) { cmp(v); }, 2, false};
  table[0xC5] = {[this]() { return zeroPage();   }, [this](uint16_t v) { cmp(v); }, 3, false};
  table[0xD5] = {[this]() { return zeroPageX();  }, [this](uint16_t v) { cmp(v); }, 4, false};
  table[0xCD] = {[this]() { return absolute();   }, [this](uint16_t v) { cmp(v); }, 4, false};
  table[0xDD] = {[this]() { return absoluteX();  }, [this](uint16_t v) { cmp(v); }, 4,  true};
  table[0xD9] = {[this]() { return absoluteY();  }, [this](uint16_t v) { cmp(v); }, 4,  true};
  table[0xC1] = {[this]() { return indirectX();  }, [this](uint16_t v) { cmp(v); }, 6, false};
  table[0xD1] = {[this]() { return indirectY();  }, [this](uint16_t v) { cmp(v); }, 5,  true};

  table[0xE0] = {[this]() { return immediate();  }, [this](uint16_t v) { cpx(v); }, 2, false};
  table[0xE4] = {[this]() { return zeroPage();   }, [this](uint16_t v) { cpx(v); }, 3, false};
  table[0xEC] = {[this]() { return absolute();   }, [this](uint16_t v) { cpx(v); }, 4, false};

  table[0xC0] = {[this]() { return immediate();  }, [this](uint16_t v) { cpy(v); }, 2, false};
  table[0xC4] = {[this]() { return zeroPage();   }, [this](uint16_t v) { cpy(v); }, 3, false};
  table[0xCC] = {[this]() { return absolute();   }, [this](uint16_t v) { cpy(v); }, 4, false};

  // -------- Branch Instructions --------
  table[0x90] = {[this]() { return relative();  }, [this](uint16_t v) { bcc(v); }, 2, false};
  table[0xB0] = {[this]() { return relative();  }, [this](uint16_t v) { bcs(v); }, 2, false};
  table[0xF0] = {[this]() { return relative();  }, [this](uint16_t v) { beq(v); }, 2, false};
  table[0xD0] = {[this]() { return relative();  }, [this](uint16_t v) { bne(v); }, 2, false};
  table[0x10] = {[this]() { return relative();  }, [this](uint16_t v) { bpl(v); }, 2, false};
  table[0x30] = {[this]() { return relative();  }, [this](uint16_t v) { bmi(v); }, 2, false};
  table[0x50] = {[this]() { return relative();  }, [this](uint16_t v) { bvc(v); }, 2, false};
  table[0x80] = {[this]() { return relative();  }, [this](uint16_t v) { bvs(v); }, 2, false};

  // -------- Jump Instructions --------
  table[0x4C] = {[this]() { return absolute();  }, [this](uint16_t v) { jmp(v); }, 3, false};
  table[0x6C] = {[this]() { return indirect();  }, [this](uint16_t v) { jmp(v); }, 5, false};

  table[0x20] = {[this]() { return absolute();  }, [this](uint16_t v) { jsr(v); }, 6, false};

  table[0x60] = {[this]() { return implied();   }, [this](uint16_t v) { rts(v); }, 6, false};

  // Uses immediate instead of implied to avoid a redundant pc++ inside brk()
  table[0x00] = {[this]() { return immediate(); }, [this](uint16_t v) { brk(v); }, 7, false};

  table[0x40] = {[this]() { return implied();   }, [this](uint16_t v) { rti(v); }, 6, false};

  // -------- Stack Instructions --------

  table[0x48] = {[this]() { return implied();   }, [this](uint16_t v) { pha(v); }, 3, false};
  table[0x68] = {[this]() { return implied();   }, [this](uint16_t v) { pla(v); }, 4, false};
  table[0x08] = {[this]() { return implied();   }, [this](uint16_t v) { php(v); }, 3, false};
  table[0x28] = {[this]() { return implied();   }, [this](uint16_t v) { plp(v); }, 4, false};
  table[0x9A] = {[this]() { return implied();   }, [this](uint16_t v) { txs(v); }, 2, false};
  table[0xBA] = {[this]() { return implied();   }, [this](uint16_t v) { tsx(v); }, 2, false};

  // -------- Flag Instructions --------
  table[0x18] = {[this]() { return implied();   }, [this](uint16_t v) { clc(v); }, 2, false};
  table[0x38] = {[this]() { return implied();   }, [this](uint16_t v) { sec(v); }, 2, false};
  table[0x58] = {[this]() { return implied();   }, [this](uint16_t v) { cli(v); }, 2, false};
  table[0x78] = {[this]() { return implied();   }, [this](uint16_t v) { sei(v); }, 2, false};
  table[0xD8] = {[this]() { return implied();   }, [this](uint16_t v) { cld(v); }, 2, false};
  table[0xF8] = {[this]() { return implied();   }, [this](uint16_t v) { sed(v); }, 2, false};
  table[0xB8] = {[this]() { return implied();   }, [this](uint16_t v) { clv(v); }, 2, false};

  // -------- Other Instructions --------
  table[0xEA] = {[this]() { return implied();   }, [this](uint16_t v) { nop(v); }, 2, false};
  // clang-format on
}

void Cpu::step() {
  uint8_t opcode = bus.read(reg.pc++);
  Instruction& instruction = table[opcode];
  if (!instruction.execute) {

    return;
  }
  extraCycles = 0;
  auto [addr, pageCrossed] = instruction.addressingMode();
  instruction.execute(addr);
  uint8_t cycles =
      instruction.cycles + ((pageCrossed && instruction.canCrossPage) ? 1 : 0) + extraCycles;
  totalCycles += cycles;
}

void Cpu::reset(bool isSoftReset) {
  if (!isSoftReset) {
    reg.a = reg.x = reg.y = 0;
    reg.sp = 0xFD;
    totalCycles = 0;
    setFlag(Carry, false);
    setFlag(Zero, false);
    setFlag(Decimal, false);
    setFlag(Overflow, false);
    setFlag(Negative, false);
  } else {
    reg.sp -= 3;
  }
  uint8_t lowPC = bus.read(0xFFFC);
  uint8_t highPC = bus.read(0xFFFD);
  reg.pc = (highPC << 8) | lowPC;
  setFlag(Interrupt, true);
}
