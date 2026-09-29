#include "bus.hpp"
#include "cpu.hpp"
#include <iostream>

// ============================================================
// Helpers
// ============================================================

bool Cpu::getFlag(StatusFlag flag) const { return (reg.status & flag) != 0; }

void Cpu::setFlag(StatusFlag flag, bool value) {
  if (value) {
    reg.status |= flag;
  } else {
    reg.status &= ~flag;
  }
}

void Cpu::updateZeroNegativeFlags(uint8_t value) {
  setFlag(Zero, value == 0);
  setFlag(Negative, (value & 0x80) != 0);
}

void Cpu::push(uint8_t byte) {
  bus.write(StackBase | reg.sp, byte);
  reg.sp--;
}

uint8_t Cpu::pull() {
  reg.sp++;
  uint8_t stackByte = bus.read(StackBase | reg.sp);
  return stackByte;
}

uint8_t Cpu::statusForPush() { return reg.status | Break | Unused; }

// ============================================================
// Access Instructions
// ============================================================

void Cpu::lda(uint16_t addr) {
  reg.a = bus.read(addr);
  updateZeroNegativeFlags(reg.a);
}

void Cpu::sta(uint16_t addr) { bus.write(addr, reg.a); }

void Cpu::ldx(uint16_t addr) {
  reg.x = bus.read(addr);
  updateZeroNegativeFlags(reg.x);
}

void Cpu::stx(uint16_t addr) { bus.write(addr, reg.x); }

void Cpu::ldy(uint16_t addr) {
  reg.y = bus.read(addr);
  updateZeroNegativeFlags(reg.y);
}

void Cpu::sty(uint16_t addr) { bus.write(addr, reg.y); }

// ============================================================
// Transfer Instructions
// ============================================================

void Cpu::tax(uint16_t /* unused */) {
  reg.x = reg.a;
  updateZeroNegativeFlags(reg.x);
}

void Cpu::txa(uint16_t /* unused */) {
  reg.a = reg.x;
  updateZeroNegativeFlags(reg.a);
}

void Cpu::tay(uint16_t /* unused */) {
  reg.y = reg.a;
  updateZeroNegativeFlags(reg.y);
}

void Cpu::tya(uint16_t /* unused */) {
  reg.a = reg.y;
  updateZeroNegativeFlags(reg.a);
}

// ============================================================
// Arithmetic Instructions
// ============================================================

void Cpu::adc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = reg.a + memoryValue + carryBit;
  uint8_t result = static_cast<uint8_t>(sum);
  bool overflow = ((reg.a ^ result) & (result ^ memoryValue) & 0x80) != 0;
  reg.a = result;

  updateZeroNegativeFlags(reg.a);
  setFlag(Carry, sum > 0xFF);
  setFlag(Overflow, overflow);
}

void Cpu::sbc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t invertedMemoryValue = ~memoryValue;
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = reg.a + invertedMemoryValue + carryBit;
  uint8_t result = static_cast<uint8_t>(sum);
  bool overflow = ((reg.a ^ result) & (result ^ invertedMemoryValue) & 0x80) != 0;
  reg.a = result;

  updateZeroNegativeFlags(reg.a);
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
  reg.x += 1;
  updateZeroNegativeFlags(reg.x);
}

void Cpu::dex(uint16_t /* unused */) {
  reg.x -= 1;
  updateZeroNegativeFlags(reg.x);
}

void Cpu::iny(uint16_t /* unused */) {
  reg.y += 1;
  updateZeroNegativeFlags(reg.y);
}

void Cpu::dey(uint16_t /* unused */) {
  reg.y -= 1;
  updateZeroNegativeFlags(reg.y);
}

// ============================================================
// Shift Instructions
// ============================================================

void Cpu::asl_a(uint16_t /* unused */) {
  bool carryBit = (reg.a & 0x80) != 0;
  reg.a = reg.a << 1;

  updateZeroNegativeFlags(reg.a);
  setFlag(Carry, carryBit);
}

void Cpu::asl(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  bool carryBit = (memoryValue & 0x80) != 0;
  memoryValue = memoryValue << 1;

  updateZeroNegativeFlags(memoryValue);
  setFlag(Carry, carryBit);
  bus.write(addr, memoryValue);
}

void Cpu::lsr_a(uint16_t /* unused */) {
  bool carryBit = (reg.a & 0x01) != 0;
  reg.a = reg.a >> 1;

  updateZeroNegativeFlags(reg.a);
  setFlag(Carry, carryBit);
}
void Cpu::lsr(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  bool carryBit = (memoryValue & 0x01) != 0;
  memoryValue = memoryValue >> 1;

  updateZeroNegativeFlags(memoryValue);
  setFlag(Carry, carryBit);
  bus.write(addr, memoryValue);
}

void Cpu::rol(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);

  bool newCarry = (memoryValue & 0x80) != 0;
  bool oldCarry = getFlag(Carry);

  memoryValue = memoryValue << 1;
  memoryValue |= oldCarry ? 1 : 0;

  updateZeroNegativeFlags(memoryValue);
  setFlag(Carry, newCarry);
  bus.write(addr, memoryValue);
}

void Cpu::rol_a(uint16_t /* unused */) {

  bool newCarry = (reg.a & 0x80) != 0;
  bool oldCarry = getFlag(Carry);

  reg.a = reg.a << 1;
  reg.a |= oldCarry ? 1 : 0;

  updateZeroNegativeFlags(reg.a);
  setFlag(Carry, newCarry);
}

void Cpu::ror(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);

  bool newCarry = (memoryValue & 0x01) != 0;
  bool oldCarry = getFlag(Carry);

  memoryValue = memoryValue >> 1;
  memoryValue |= (oldCarry ? 0x80 : 0);

  updateZeroNegativeFlags(memoryValue);
  setFlag(Carry, newCarry);
  bus.write(addr, memoryValue);
}

void Cpu::ror_a(uint16_t /* unused */) {
  bool newCarry = (reg.a & 0x01) != 0;
  bool oldCarry = getFlag(Carry);

  reg.a = reg.a >> 1;
  reg.a |= (oldCarry ? 0x80 : 0);

  updateZeroNegativeFlags(reg.a);
  setFlag(Carry, newCarry);
}

// ============================================================
// Bitwise Instructions
// ============================================================

void Cpu::and_a(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  reg.a &= memoryValue;
  updateZeroNegativeFlags(reg.a);
}

void Cpu::ora(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  reg.a |= memoryValue;
  updateZeroNegativeFlags(reg.a);
}

void Cpu::eor(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  reg.a ^= memoryValue;
  updateZeroNegativeFlags(reg.a);
}

void Cpu::bit(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = reg.a & memoryValue;

  setFlag(Zero, result == 0);
  setFlag(Overflow, (memoryValue & 0x40) != 0);
  setFlag(Negative, (memoryValue & 0x80) != 0);
}

// ============================================================
// Compare Instructions
// ============================================================

void Cpu::cmp(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = reg.a - memoryValue;

  setFlag(Carry, reg.a >= memoryValue);
  setFlag(Zero, reg.a == memoryValue);
  setFlag(Negative, (result & 0x80) != 0);
}

void Cpu::cpx(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = reg.x - memoryValue;

  setFlag(Carry, reg.x >= memoryValue);
  setFlag(Zero, reg.x == memoryValue);
  setFlag(Negative, (result & 0x80) != 0);
}

void Cpu::cpy(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = reg.y - memoryValue;

  setFlag(Carry, reg.y >= memoryValue);
  setFlag(Zero, reg.y == memoryValue);
  setFlag(Negative, (result & 0x80) != 0);
}

// ============================================================
// Branch Instructions
// ============================================================

void Cpu::branch(bool condition, uint16_t addr) {
  if (!condition) {
    return;
  }
  extraCycles += 1;
  bool pageCrossed = (reg.pc & 0xFF00) != (addr & 0xFF00);
  if (pageCrossed) {
    extraCycles += 1;
  }
  reg.pc = addr;
}

void Cpu::bcc(uint16_t addr) { branch(!getFlag(Carry), addr); }

void Cpu::bcs(uint16_t addr) { branch(getFlag(Carry), addr); }

void Cpu::beq(uint16_t addr) { branch(getFlag(Zero), addr); }

void Cpu::bne(uint16_t addr) { branch(!getFlag(Zero), addr); }

void Cpu::bpl(uint16_t addr) { branch(!getFlag(Negative), addr); }

void Cpu::bmi(uint16_t addr) { branch(getFlag(Negative), addr); }

void Cpu::bvc(uint16_t addr) { branch(!getFlag(Overflow), addr); }

void Cpu::bvs(uint16_t addr) { branch(getFlag(Overflow), addr); }

// ============================================================
// Jump Instructions
// ============================================================

void Cpu::jmp(uint16_t addr) { reg.pc = addr; }

void Cpu::jsr(uint16_t addr) {
  uint16_t returnAddr = reg.pc - 1;
  uint8_t highByte = returnAddr >> 8;
  uint8_t lowByte = returnAddr & 0xFF;

  push(highByte);
  push(lowByte);
  reg.pc = addr;
}

void Cpu::rts(uint16_t /* unused */) {
  uint8_t lowByte = pull();
  uint8_t highByte = pull();
  uint16_t addr = (highByte << 8) | lowByte;
  reg.pc = addr + 1;
}

void Cpu::brk(uint16_t /* unused */) {
  uint8_t highByte = reg.pc >> 8;
  uint8_t lowByte = reg.pc & 0xFF;
  push(highByte);
  push(lowByte);

  // Flag only exists in the byte pushed to the stack, not real state in the CPU
  push(statusForPush());
  setFlag(Interrupt, true);

  uint8_t low = bus.read(0xFFFE);
  uint8_t high = bus.read(0xFFFF);
  reg.pc = (high << 8) | low;
}

void Cpu::rti(uint16_t /* unused */) {
  reg.status = pull();
  uint8_t lowByte = pull();
  uint8_t highByte = pull();
  reg.pc = (highByte << 8) | lowByte;
}

// ============================================================
// Stack Instructions
// ============================================================

void Cpu::pha(uint16_t /* unused */) { push(reg.a); }

void Cpu::pla(uint16_t /* unused */) {
  reg.a = pull();
  updateZeroNegativeFlags(reg.a);
}

void Cpu::php(uint16_t /* unused */) { push(statusForPush()); }

void Cpu::plp(uint16_t /* unused */) { reg.status = pull(); }

void Cpu::txs(uint16_t /* unused */) { reg.sp = reg.x; }

void Cpu::tsx(uint16_t /* unused */) {
  reg.x = reg.sp;
  updateZeroNegativeFlags(reg.x);
}

// ============================================================
// Flags Instructions
// ============================================================

void Cpu::clc(uint16_t /* unused */) { setFlag(Carry, false); }

void Cpu::sec(uint16_t /* unused */) { setFlag(Carry, true); }

void Cpu::cli(uint16_t /* unused */) { setFlag(Interrupt, false); }

void Cpu::sei(uint16_t /* unused */) { setFlag(Interrupt, true); }

void Cpu::cld(uint16_t /* unused */) { setFlag(Decimal, false); }

void Cpu::sed(uint16_t /* unused */) { setFlag(Decimal, true); }

void Cpu::clv(uint16_t /* unused */) { setFlag(Overflow, false); }

// ============================================================
// Other Instructions
// ============================================================

void Cpu::nop(uint16_t /* unused */) { return; }
