#include "bus.hpp"
#include "cartridge.hpp"

// 2KiB of internal RAM, mirrored every $0800 up to $1FFF
constexpr uint16_t RAM_END = 0x1FFF;
constexpr uint16_t RAM_MIRROR_MASK = 0x07FF;

// PPU registers, mirrored through $3FFF
constexpr uint16_t PPU_END = 0x3FFF;

// APU and controller I/O (the cartridge owns everything above this)
constexpr uint16_t APU_IO_END = 0x401F;

void Bus::write(uint16_t addr, uint8_t data) {
  if (addr <= RAM_END) {
    memory[addr & RAM_MIRROR_MASK] = data;
  } else if (addr <= PPU_END) {
    // TODO: PPU stuff
    return;
  } else if (addr <= APU_IO_END) {
    // TODO: APU and controller
    return;
  }
}

uint8_t Bus::read(uint16_t addr) const {
  if (addr <= RAM_END) {
    return memory[addr & RAM_MIRROR_MASK];
  } else if (addr <= PPU_END) {
    // TODO: PPU stuff
    return 0;
  } else if (addr <= APU_IO_END) {
    // TODO: APU and controller
    return 0;
  } else if (cartridge != nullptr) {
    return cartridge->read(addr);
  }
  return 0;
}
