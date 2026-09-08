#include "bus.hpp"
#include "cpu.hpp"
#include <catch2/catch_test_macros.hpp>

// -------- LDA --------

TEST_CASE("LDA immediate loads a value into A") {
  Bus bus;
  Cpu cpu(bus);

  bus.write(0x200, 0xA9);
  bus.write(0x201, 0x05);
  cpu.setPC(0x200);

  cpu.step();

  REQUIRE(cpu.getA() == 0x05);
  REQUIRE(cpu.getFlag(Cpu::Zero) == false);
  REQUIRE(cpu.getFlag(Cpu::Negative) == false);
}

TEST_CASE("LDA immediate sets zero flag when loading zero") {
  Bus bus;
  Cpu cpu(bus);

  bus.write(0x200, 0xA9);
  bus.write(0x201, 0x00);
  cpu.setPC(0x200);
  cpu.step();

  REQUIRE(cpu.getA() == 0x00);
  REQUIRE(cpu.getFlag(Cpu::Zero) == true);
  REQUIRE(cpu.getFlag(Cpu::Negative) == false);
}

TEST_CASE("LDA immediate sets negative flag when loading negative") {
  Bus bus;
  Cpu cpu(bus);

  bus.write(0x200, 0xA9);
  bus.write(0x201, 0x80);
  cpu.setPC(0x200);
  cpu.step();

  REQUIRE(cpu.getA() == 0x80);
  REQUIRE(cpu.getFlag(Cpu::Zero) == false);
  REQUIRE(cpu.getFlag(Cpu::Negative) == true);
}
