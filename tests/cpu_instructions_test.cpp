#include "bus.hpp"
#include "cpu.hpp"
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

struct CpuFixture {
  Bus bus;
  Cpu cpu;
  uint16_t writeAddr = 0x200;

  CpuFixture() : cpu(bus) { cpu.setPC(0x200); }

  /// @brief Allows to load multiple bytes
  /// @param bytes The list of bytes to load
  void loadBytes(std::initializer_list<uint8_t> bytes) {
    for (uint8_t b : bytes) {
      bus.write(writeAddr++, b);
    }
  }
};

// ============================================================
// Access Instructions
// ============================================================

TEST_CASE_METHOD(CpuFixture, "LDA immediate loads a value into A") {
  loadBytes({0xA9, 0x05});
  cpu.step();

  REQUIRE(cpu.getA() == 0x05);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
}

TEST_CASE_METHOD(CpuFixture, "LDA immediate sets zero flag when loading zero") {
  loadBytes({0xA9, 0x00});
  cpu.step();

  REQUIRE(cpu.getA() == 0x00);
  REQUIRE(cpu.getFlag(Cpu::Zero));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
}

TEST_CASE_METHOD(CpuFixture, "LDA immediate sets negative flag when loading negative") {
  loadBytes({0xA9, 0x80});
  cpu.step();

  REQUIRE(cpu.getA() == 0x80);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
  REQUIRE(cpu.getFlag(Cpu::Negative) == true);
}

// ============================================================
// Arithmetic Instructions
// ============================================================

TEST_CASE_METHOD(CpuFixture, "ADC adds two positives with no carry or overflow") {
  cpu.setA(0x010);
  loadBytes({0x69, 0x20});
  cpu.step();

  REQUIRE(cpu.getA() == 0x30);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Overflow));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Carry));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
}

TEST_CASE_METHOD(CpuFixture, "ADC sets carry on unsigned overflow") {
  cpu.setA(0xFF);
  loadBytes({0x69, 0x01});
  cpu.step();

  REQUIRE(cpu.getA() == 0x00);
  REQUIRE(cpu.getFlag(Cpu::Carry));
  REQUIRE(cpu.getFlag(Cpu::Zero));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Overflow));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
}

TEST_CASE_METHOD(CpuFixture, "ADC sets overflow when two positives creates a negative ") {
  cpu.setA(0x50);
  loadBytes({0x69, 0x50});
  cpu.step();

  REQUIRE(cpu.getA() == 0xA0);
  REQUIRE(cpu.getFlag(Cpu::Overflow));
  REQUIRE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Carry));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

TEST_CASE_METHOD(CpuFixture, "SBC subtracts with no underflow") {
  cpu.setA(0x50);

  // Does not subtract when carry is set
  cpu.setFlag(Cpu::Carry, true);
  loadBytes({0xE9, 0x30});
  cpu.step();

  REQUIRE(cpu.getA() == 0x20);
  REQUIRE(cpu.getFlag(Cpu::Carry) == true); // no underflow
  REQUIRE_FALSE(cpu.getFlag(Cpu::Overflow));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

TEST_CASE_METHOD(CpuFixture, "SBC subtracts one when carry is cleared") {
  cpu.setA(0x50);

  // CLC, Subtracts one more when carry is clear
  cpu.setFlag(Cpu::Carry, false);
  loadBytes({0xE9, 0x30});
  cpu.step();

  REQUIRE(cpu.getA() == 0x1F);
  REQUIRE(cpu.getFlag(Cpu::Carry) == true); // no underflow
  REQUIRE_FALSE(cpu.getFlag(Cpu::Overflow));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

TEST_CASE_METHOD(CpuFixture, "SBC sets carry when underflows") {
  cpu.setA(0x30);

  // SEC dont clear
  cpu.setFlag(Cpu::Carry, true);
  loadBytes({0xE9, 0x40});
  cpu.step();

  REQUIRE(cpu.getA() == 0xF0);
  REQUIRE(cpu.getFlag(Cpu::Negative) == true);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Carry));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Overflow));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

TEST_CASE_METHOD(CpuFixture, "INX increases memory by one") {
  loadBytes({0xE8});
  cpu.step();

  REQUIRE(cpu.getX() == 0x01);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

// ============================================================
// Shift Instructions
// ============================================================

TEST_CASE_METHOD(CpuFixture, "ASL A shifts all bits left") {

  cpu.setA(0x71); // 01110001
  loadBytes({0x0A});
  cpu.step();

  REQUIRE(cpu.getA() == 0xE2); // 11100010
  REQUIRE(cpu.getFlag(Cpu::Negative) == true);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Carry));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

TEST_CASE_METHOD(CpuFixture, "ASL shifts all bits left") {
  bus.write(0x0010, 0xAA); // 10101010
  loadBytes({0x06, 0x10});
  cpu.step();

  REQUIRE(bus.read(0x0010) == 0x54); // 01010100
  REQUIRE(cpu.getFlag(Cpu::Carry) == true);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

TEST_CASE_METHOD(CpuFixture, "ROL rotates all bits left") {
  cpu.setFlag(Cpu::Carry, false);
  bus.write(0x0010, 0xAA); // 10101010
  loadBytes({0x26, 0x10});
  cpu.step();
  // std::cout << std::bitset<8>(bus.read(0x0010)) << std::endl;

  REQUIRE(bus.read(0x0010) == 0x54); // 01010100
  REQUIRE(cpu.getFlag(Cpu::Carry) == true);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}

// ============================================================
// Jump Instructions
// ============================================================

TEST_CASE_METHOD(CpuFixture, "JMP absolute jumps to the correct address") {
  loadBytes({0x4C, 0x00, 0x03}); // JMP 0x0300
  cpu.step();

  REQUIRE(cpu.getPC() == 0x0300);
}

TEST_CASE_METHOD(CpuFixture, "JMP indirect jumps to the address stored at the pointer") {
  bus.write(0x0350, 0x34); // at 0x0350 write 0x1234
  bus.write(0x0351, 0x12);
  loadBytes({0x6C, 0x50, 0x03}); // JMP 0x0350
  cpu.step();

  REQUIRE(cpu.getPC() == 0x1234);
}

TEST_CASE_METHOD(CpuFixture, "JMP indirect wraps when the address ends in 0xFF") {
  bus.write(0x03FF, 0x34);       // address ends in FF
  bus.write(0x0300, 0x12);       // wrap around to the start of the page
  bus.write(0x0400, 0x99);       // Next page (usual case of ptr + 1)
  loadBytes({0x6C, 0xFF, 0x03}); // JMP 0x03FF
  cpu.step();

  REQUIRE(cpu.getPC() == 0x1234);
}

TEST_CASE_METHOD(CpuFixture, "JSR pushes the return address and jumps to the new target") {
  cpu.setPC(0x1000);
  writeAddr = 0x1000; // overwrite because 0x200 jumps to 0x0202, the high and low bytes are
                      // identical and cant differentiate
  loadBytes({0x20, 0x00, 0x30});
  cpu.step();

  REQUIRE(cpu.getPC() == 0x3000);
  REQUIRE(cpu.getSP() == 0xFE);      // stack decreases by one
  REQUIRE(bus.read(0x0100) == 0x10); // 0x1000 jumps to 0x1002
  REQUIRE(bus.read(0x01FF) == 0x02);
}

TEST_CASE_METHOD(CpuFixture, "BRK jumps to the address at 0xFFFE") {
  bus.write(0xFFFE, 0x34);
  bus.write(0xFFFF, 0x12);
  loadBytes({0x00, 0x00});
  cpu.step();

  REQUIRE(cpu.getPC() == 0x1234);
}

TEST_CASE_METHOD(CpuFixture, "BRK pushes the return address after skipping the padding byte") {
  cpu.setPC(0x1000);
  writeAddr = 0x1000;
  loadBytes({0x00, 0x00}); // BRK + padding byte
  cpu.step();

  // return address = 0x1002
  REQUIRE(bus.read(0x0100) == 0x10);
  REQUIRE(bus.read(0x01FF) == 0x02);
}

// ============================================================
// Flag Instructions
// ============================================================

TEST_CASE_METHOD(CpuFixture, "CLC clears the carry flag") {
  cpu.setFlag(Cpu::Carry, true);
  loadBytes({0x18});
  cpu.step();

  REQUIRE_FALSE(cpu.getFlag(Cpu::Carry));
}

TEST_CASE_METHOD(CpuFixture, "SEC sets the carry flag") {
  cpu.setFlag(Cpu::Carry, false);
  loadBytes({0x38});
  cpu.step();

  REQUIRE(cpu.getFlag(Cpu::Carry) == true);
}

TEST_CASE_METHOD(CpuFixture, "CLI clears the interrupt disable flag") {
  cpu.setFlag(Cpu::Interrupt, true);
  loadBytes({0x58});
  cpu.step();

  REQUIRE_FALSE(cpu.getFlag(Cpu::Interrupt));
}

TEST_CASE_METHOD(CpuFixture, "SEI sets the interrupt disable flag") {
  cpu.setFlag(Cpu::Interrupt, false);
  loadBytes({0x78});
  cpu.step();

  REQUIRE(cpu.getFlag(Cpu::Interrupt) == true);
}

TEST_CASE_METHOD(CpuFixture, "CLD clears the decimal flag") {
  cpu.setFlag(Cpu::Decimal, true);
  loadBytes({0xD8});
  cpu.step();

  REQUIRE_FALSE(cpu.getFlag(Cpu::Decimal));
}

TEST_CASE_METHOD(CpuFixture, "SED sets the decimal flag") {
  cpu.setFlag(Cpu::Decimal, false);
  loadBytes({0xF8});
  cpu.step();

  REQUIRE(cpu.getFlag(Cpu::Decimal) == true);
}

TEST_CASE_METHOD(CpuFixture, "CLV clears the overflow flag") {
  cpu.setFlag(Cpu::Overflow, true);
  loadBytes({0xB8});
  cpu.step();

  REQUIRE_FALSE(cpu.getFlag(Cpu::Overflow));
}