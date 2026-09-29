#pragma once

#include "bus.hpp"
#include "cpu.hpp"
#include "initializer_list"

class CpuFixture {
protected:
  Bus bus;
  Cpu cpu;
  uint16_t writeAddr = 0x200;

  CpuFixture() : cpu(bus) { setPC(0x200); }

  /// @brief Allows to load multiple bytes
  /// @param bytes The list of bytes to load
  void loadBytes(std::initializer_list<uint8_t> bytes) {
    for (uint8_t b : bytes) {
      bus.write(writeAddr++, b);
    }
  }

  void setA(uint8_t value) {
    auto state = cpu.state();
    state.a = value;
    cpu.setState(state);
  }

  void setX(uint8_t value) {
    auto state = cpu.state();
    state.x = value;
    cpu.setState(state);
  }

  void setPC(uint16_t value) {
    auto state = cpu.state();
    state.pc = value;
    cpu.setState(state);
  }
  
};