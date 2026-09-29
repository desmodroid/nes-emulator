#include "cartridge.hpp"
#include <stdexcept>

INesHeader parseHeader(const std::vector<uint8_t>& data) {
  if (data.size() < 16) {
    throw std::invalid_argument("iNes header must be at least 16 bytes");
  }
  if (data[0] != 'N' || data[1] != 'E' || data[2] != 'S' || data[3] != 0x1A) {
    throw std::invalid_argument("Invalid NES file: missing magic bytes");
  }

  INesHeader header;
  header.prgRomSizeBytes = static_cast<size_t>(data[4] * 16384);
  header.chrRomSizeBytes = static_cast<size_t>(data[5] * 8192);
  header.verticalMirorring = (data[6] & 0x01) != 0;
  header.hasBatteryBackedRam = (data[6] & 0x02) != 0;
  header.hasTrainer = (data[6] & 0x04) != 0;
  uint8_t lowNybbleMapper = (data[6] & 0xF0);
  uint8_t highNybbleMapper = (data[7] & 0xF0);
  header.mapper = highNybbleMapper | (lowNybbleMapper >> 4);

  return header;
}

Cartridge::Cartridge(const std::vector<uint8_t>& data) : header(parseHeader(data)) {
  size_t offset = 16;
  if (header.hasTrainer) {
    offset += 512;
  }
  if (data.size() < offset + header.prgRomSizeBytes + header.chrRomSizeBytes) {
    throw std::invalid_argument("ROM size is smaller what the header claims");
  }

  prgRom.assign(data.begin() + offset, data.begin() + offset + header.prgRomSizeBytes);
  offset += header.prgRomSizeBytes;
  chrRom.assign(data.begin() + offset, data.begin() + offset + header.chrRomSizeBytes);
}