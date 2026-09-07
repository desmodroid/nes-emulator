#pragma once

#include <cstdint>
#include <functional>

// Forward declaration, cpu doesnt need the full Bus def
class Bus;

/// Represents the NES 6502 CPU
class Cpu {
public:
  /// @brief Status flags stored in the register
  enum StatusFlag : uint8_t {
    Carry = 1 << 0,     // Set when an addition produces a carry
    Zero = 1 << 1,      // Set when the result is zero
    Interrupt = 1 << 2, // Set to disable maskable interrupts
    Decimal = 1 << 3,   // Decimal mode (unused on the NES)
    Break = 1 << 4,     // Set when status is pushed by BRK/PHP
    Unused = 1 << 5,    // Always pushed as 1
    Overflow = 1 << 6,  // Set when signed arithmetic overflows
    Negative = 1 << 7,  // Set when bit 7 of the result is 1
  };

  /// @brief Creates a 6502 CPU connected to the bus
  /// @param bus The bus used for memory access
  explicit Cpu(Bus& bus);

  /// @brief Checks if a flag has been set
  /// @param flag The status flag to check
  /// @return True if the flag is set, false otherwise
  bool getFlag(StatusFlag flag) const;

  /// @brief Set a status flag to true or false
  /// @param flag The status flag to set
  /// @param value The boolean the flag will be set
  void setFlag(StatusFlag flag, bool value);

  /// @brief Fetches an opcode, decodes it and executes it
  void step();

private:
  Bus& bus;

  // Registers
  uint8_t a = 0;      // Accumulator
  uint8_t x = 0;      // X index register
  uint8_t y = 0;      // Y index register
  uint16_t pc = 0;    // Program counter
  uint8_t sp = 0;     // Stack pointer
  uint8_t status = 0; // Status register

  /// @brief Opcode table with its addressing mode, execution and number of
  /// cycles.
  struct Instruction {
    std::function<uint8_t()> addressingMode;
    std::function<void(uint8_t)> execute;
    uint8_t cycles = 0;
  };

  /// @brief Lookup table of all the possible opcode values
  std::array<Instruction, 256> table;

  /// @brief Populates table with all implemented opcodes
  void buildTable();

  /// @brief Immediate addressing mode: take the next byte in the instruction
  /// stream
  /// @return The fetched operand byte
  uint8_t immediate();

  /// @brief Zero page addressing mode: read the operand from an address in the
  /// first 256 bytes of memory 
  /// @return The fetched operand byte
  uint8_t zeroPage();

  /// @brief Loads a memory value into the accunmulator
  /// @param value The byte to load
  void lda(uint8_t value);
};