#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

struct INesHeader {
  size_t prgRomSizeBytes;
  size_t chrRomSizeBytes;
  uint8_t mapper;
  bool verticalMirorring;
  bool hasBatteryBackedRam;
  bool hasTrainer;
};

/// @brief Validates and parses the iNes header from a ROM
/// @param data The raw bytes of a nes file
/// @return The parsed header
INesHeader parseHeader(const std::vector<uint8_t>& data);

class Cartridge {
public:
  /// @brief Constructor for a cartdridge
  /// @param data The raw bytes of a nes file
  explicit Cartridge(const std::vector<uint8_t>& data);

  /// @brief Gets the parsed header
  /// @return The cartridges header
  const INesHeader& getHeader() const { return header; }

  /// @brief Gets the catridges PRG ROM data
  /// @return The raw PRG ROM bytes
  const std::vector<uint8_t>& getPrgRom() const { return prgRom; }

  /// @brief Gets the cartidges CHR ROM data
  /// @return The raw CHR ROM bytes
  const std::vector<uint8_t>& getChrRom() const { return chrRom; }

  uint8_t read(uint16_t) const;

private:
  INesHeader header;
  std::vector<uint8_t> prgRom;
  std::vector<uint8_t> chrRom;
};
