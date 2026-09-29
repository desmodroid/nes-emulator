#include "bus.hpp"
#include "cpu.hpp"
#include <iostream>

// ============================================================
// Helpers
// ============================================================

void Cpu::push(uint8_t byte) {
  bus.write(StackBase | sp, byte);
  sp--;
}

uint8_t Cpu::pull() {
  sp++;
  uint8_t stackByte = bus.read(StackBase | sp);
  return stackByte;
}

// ============================================================
// Access Instructions
// ============================================================

void Cpu::lda(uint16_t addr) {
  a = bus.read(addr);
  updateZeroNegativeFlags(a);
}

void Cpu::sta(uint16_t addr) { bus.write(addr, a); }

void Cpu::ldx(uint16_t addr) {
  x = bus.read(addr);
  updateZeroNegativeFlags(x);
}

void Cpu::stx(uint16_t addr) { bus.write(addr, x); }

void Cpu::ldy(uint16_t addr) {
  y = bus.read(addr);
  updateZeroNegativeFlags(y);
}

void Cpu::sty(uint16_t addr) { bus.write(addr, y); }

// ============================================================
// Transfer Instructions
// ============================================================

void Cpu::tax(uint16_t /* unused */) {
  x = a;
  updateZeroNegativeFlags(x);
}

void Cpu::txa(uint16_t /* unused */) {
  a = x;
  updateZeroNegativeFlags(a);
}

void Cpu::tay(uint16_t /* unused */) {
  y = a;
  updateZeroNegativeFlags(y);
}

void Cpu::tya(uint16_t /* unused */) {
  a = y;
  updateZeroNegativeFlags(a);
}

// ============================================================
// Arithmetic Instructions
// ============================================================

void Cpu::adc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = a + memoryValue + carryBit;
  uint8_t result = static_cast<uint8_t>(sum);
  bool overflow = ((a ^ result) & (result ^ memoryValue) & 0x80) != 0;
  a = result;

  updateZeroNegativeFlags(a);
  setFlag(Carry, sum > 0xFF);
  setFlag(Overflow, overflow);
}

void Cpu::sbc(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t invertedMemoryValue = ~memoryValue;
  uint8_t carryBit = getFlag(Carry) ? 1 : 0;
  uint16_t sum = a + invertedMemoryValue + carryBit;
  uint8_t result = static_cast<uint8_t>(sum);
  bool overflow = ((a ^ result) & (result ^ invertedMemoryValue) & 0x80) != 0;
  a = result;

  updateZeroNegativeFlags(a);
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
  x += 1;
  updateZeroNegativeFlags(x);
}

void Cpu::dex(uint16_t /* unused */) {
  x -= 1;
  updateZeroNegativeFlags(x);
}

void Cpu::iny(uint16_t /* unused */) {
  y += 1;
  updateZeroNegativeFlags(y);
}

void Cpu::dey(uint16_t /* unused */) {
  y -= 1;
  updateZeroNegativeFlags(y);
}

// ============================================================
// Shift Instructions
// ============================================================

void Cpu::asl_a(uint16_t /* unused */) {
  bool carryBit = (a & 0x80) != 0;
  a = a << 1;

  updateZeroNegativeFlags(a);
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
  bool carryBit = (a & 0x01) != 0;
  a = a >> 1;

  updateZeroNegativeFlags(a);
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

  bool newCarry = (a & 0x80) != 0;
  bool oldCarry = getFlag(Carry);

  a = a << 1;
  a |= oldCarry ? 1 : 0;

  updateZeroNegativeFlags(a);
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
  bool newCarry = (a & 0x01) != 0;
  bool oldCarry = getFlag(Carry);

  a = a >> 1;
  a |= (oldCarry ? 0x80 : 0);

  updateZeroNegativeFlags(a);
  setFlag(Carry, newCarry);
}

// ============================================================
// Bitwise Instructions
// ============================================================

void Cpu::and_a(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  a &= memoryValue;
  updateZeroNegativeFlags(a);
}

void Cpu::ora(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  a |= memoryValue;
  updateZeroNegativeFlags(a);
}

void Cpu::eor(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  a ^= memoryValue;
  updateZeroNegativeFlags(a);
}

void Cpu::bit(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = a & memoryValue;

  setFlag(Zero, result == 0);
  setFlag(Overflow, (memoryValue & 0x40) != 0);
  setFlag(Negative, (memoryValue & 0x80) != 0);
}

// ============================================================
// Compare Instructions
// ============================================================

void Cpu::cmp(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = a - memoryValue;

  setFlag(Carry, a >= memoryValue);
  setFlag(Zero, a == memoryValue);
  setFlag(Negative, (result & 0x80) != 0);
}

void Cpu::cpx(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = x - memoryValue;

  setFlag(Carry, x >= memoryValue);
  setFlag(Zero, x == memoryValue);
  setFlag(Negative, (result & 0x80) != 0);
}

void Cpu::cpy(uint16_t addr) {
  uint8_t memoryValue = bus.read(addr);
  uint8_t result = y - memoryValue;

  setFlag(Carry, y >= memoryValue);
  setFlag(Zero, y == memoryValue);
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
  bool pageCrossed = (pc & 0xFF00) != (addr & 0xFF00);
  if (pageCrossed) {
    extraCycles += 1;
  }
  pc = addr;
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

void Cpu::jmp(uint16_t addr) { pc = addr; }

void Cpu::jsr(uint16_t addr) {
  uint16_t returnAddr = pc - 1;
  uint8_t highByte = returnAddr >> 8;
  uint8_t lowByte = returnAddr & 0xFF;

  push(highByte);
  push(lowByte);
  pc = addr;
}

void Cpu::rts(uint16_t /* unused */) {
  uint8_t lowByte = pull();
  uint8_t highByte = pull();
  uint16_t addr = (highByte << 8) | lowByte;
  pc = addr + 1;
}

void Cpu::brk(uint16_t /* unused */) {
  uint8_t highByte = pc >> 8;
  uint8_t lowByte = pc & 0xFF;
  push(highByte);
  push(lowByte);

  // Flag only exists in the byte pushed to the stack, not real state in the CPU
  uint8_t pushedStatus = status | Break | Unused;
  push(pushedStatus);
  setFlag(Interrupt, true);

  uint8_t low = bus.read(0xFFFE);
  uint8_t high = bus.read(0xFFFF);
  pc = (high << 8) | low;
}