#include <bus.hpp>
#include <cartridge.hpp>
#include <cpu.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<uint8_t> readFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    throw std::invalid_argument("Could not open NES rom");
  }
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <rom.nes>" << std::endl;
    return 1;
  }
  try {
    Cartridge cartridge(readFile(argv[1]));
    Bus bus;
    Cpu cpu(bus);
    cpu.reset(false);

    while (true) {
      cpu.step();
    }
  } catch (const std::exception& e) {
    std::cerr << "Failed to load rom: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}