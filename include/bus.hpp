#pragma once

#include <array>
#include <cstdint>

/// @brief Represents the 6502 bus and 64KiB 
class Bus {
public:
  /// @brief Writes a byte to the specified memory address
  /// @param addr The 16-bit memory address to write to
  /// @param data The 8-bit value to write
  void write(uint16_t addr, uint8_t data);

  /// @brief Reads a byte from the specified memory address
  /// @param addr The 16-bit memory address to write to
  /// @return The 8-bit value stored at the address
  uint8_t read(uint16_t addr) const;

private:
  /// @brief 64KiB of memory
  std::array<uint8_t, 65536> memory{};
};