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

void printTraceLine(const Cpu& cpu, const Bus& bus) {
  // clang-format off

  CpuState s = cpu.state();
  std::ofstream log("nestest.log");
  std::cout << std::hex << std::uppercase << std::setfill('0')
            << std::setw(4) << s.pc << "  "
            << "A:" << std::setw(2) << (int)s.a
            << " X:" << std::setw(2) << (int)s.x
            << " Y:" << std::setw(2) << (int)s.y
            << " P:" << std::setw(2) << (int)s.status
            << " SP:" << std::setw(2) << (int)s.sp
            << " CYC:" << std::dec << s.totalCycles << "\n";
  // clang-format on
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <rom.nes>" << std::endl;
    return 1;
  }
  try {
    Cartridge cartridge(readFile(argv[1]));
    Bus bus;
    bus.connectCartridge(&cartridge);
    Cpu cpu(bus);
    cpu.reset(false);

    // NES TEST
    // CpuState state = cpu.state();
    // state.pc = 0xC000;
    // cpu.setState(state);

    while (true) {
      // printTraceLine(cpu, bus);
      cpu.step();
    }
  } catch (const std::exception& e) {
    std::cerr << "Failed to load rom: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}