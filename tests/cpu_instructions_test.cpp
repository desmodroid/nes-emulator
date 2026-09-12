#include "bus.hpp"
#include "cpu.hpp"
#include <catch2/catch_test_macros.hpp>

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
  loadBytes({0xE8, 0x01});
  cpu.step();

  REQUIRE(cpu.getX() == 0x01);
  REQUIRE_FALSE(cpu.getFlag(Cpu::Negative));
  REQUIRE_FALSE(cpu.getFlag(Cpu::Zero));
}
