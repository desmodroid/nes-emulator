#include "bus.hpp"
#include "cartridge.hpp"

void Bus::write(uint16_t addr, uint8_t data) {
  if (addr <= 0x1FFF) {
    memory[addr & 0x07FF] = data;
  } else if (addr <= 0x3FFF) {
    // TODO: PPU stuff
    return;
  } else if (addr <= 0x401F) {
    // TODO: APU and controller
    return;
  }
}

uint8_t Bus::read(uint16_t addr) const {
  if (addr <= 0x1FFF) {
    return memory[addr & 0x07FF];
  } else if (addr <= 0x3FFF) {
    // TODO: PPU stuff
    return 0;
  } else if (addr <= 0x401F) {
    // TODO: APU and controller
    return 0;
  } else if (addr >= 0x4020 && addr <= 0xFFFF) {
    if (cartridge != nullptr) {
      return cartridge->read(addr);
    }
  }
  return 0;
}