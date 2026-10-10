#pragma once

#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include <initializer_list>
#include <optional>
#include <vector>

class CpuFixture {
protected:
  Bus bus;
  Cpu cpu;
  uint16_t writeAddr = 0x200;
  std::optional<Cartridge> cartridge;

  CpuFixture() : cpu(bus) { setPC(0x200); }

  /// @brief Allows to load multiple bytes
  /// @param bytes The list of bytes to load
  void loadBytes(std::initializer_list<uint8_t> bytes) {
    for (uint8_t b : bytes) {
      bus.write(writeAddr++, b);
    }
  }

  /// @brief Builds a minimal 16KB PRG ROM cartridge and connects it to the bus
  /// @param prgBytes Pairs of CPU address ($8000-$FFFF) and the byte to place there
  /// @return The attached cartridge
  /// @note Addresses are mirrored into the 16KB bank, so $C000-$FFFF maps onto $8000-$BFFF
  Cartridge& attachCartridge(std::initializer_list<std::pair<uint16_t, uint8_t>> prgBytes) {
    std::vector<uint8_t> data(16 + 16384, 0);
    data[0] = 'N';
    data[1] = 'E';
    data[2] = 'S';
    data[3] = 0x1A;
    data[4] = 1; // 1 * 16KB PRG-ROM
    data[5] = 0; // no CHR-ROM

    for (auto& [addr, value] : prgBytes) {
      size_t offset = 16 + ((addr - 0x8000) % 16384);
      data[offset] = value;
    }

    cartridge.emplace(data);
    bus.connectCartridge(&cartridge.value());
    return cartridge.value();
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