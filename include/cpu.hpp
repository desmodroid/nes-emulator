#pragma once

#include <array>
#include <cstdint>
#include <functional>

// Forward declaration, cpu doesnt need the full Bus def
class Bus;

/// Represents the NES 6502 CPU
class Cpu {
public:
  // -------- Getters and Setters for testing --------
  
  /// @brief Get accumulator register
  /// @return The accumulator register, a
  uint8_t getA() const { return a; }

  /// @brief Sets the program counter
  /// @param addr The address to set as the new program counter
  void setPC(uint16_t addr) { pc = addr; }

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

  // -------- Registers --------
  uint8_t a = 0;      // Accumulator
  uint8_t x = 0;      // X index register
  uint8_t y = 0;      // Y index register
  uint16_t pc = 0;    // Program counter
  uint8_t sp = 0;     // Stack pointer
  uint8_t status = 0; // Status register

  /// @brief Opcode table with its addressing mode, execution and number of
  /// cycles.
  struct Instruction {
    std::function<std::pair<uint16_t, bool>()> addressingMode;
    std::function<void(uint16_t)> execute;
    uint8_t cycles = 0;
  };

  /// @brief Lookup table of all the possible opcode values
  std::array<Instruction, 256> table;

  /// @brief Populates table with all implemented opcodes
  void buildTable();

  // -------- Addressing Modes --------

  /// @brief Immediate addressing mode
  /// @return The address of the operand
  std::pair<uint16_t, bool> immediate();

  /// @brief Zero page addressing mode
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> zeroPage();

  /// @brief Zero page X addressing mode: adds the X register to the address
  /// provided by the operand
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> zeroPageX();

  /// @brief Absolute addressing mode
  /// @return The 16 bit effective address of the operand and if the page has
  /// been crossed
  std::pair<uint16_t, bool> absolute();

  /// @brief Absolute X addressing mode: adds the X register to the address
  /// provided by the operand
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> absoluteX();

  /// @brief Absolute Y addressing mode: adds the Y register to the address
  /// provided by the operand
  /// @return The 16 bit effective address of the operand and if the page has
  /// been crossed
  std::pair<uint16_t, bool> absoluteY();

  /// @brief Indirect X addressing mode: reads a 16 bit address from a zero page
  /// pointer then adds Y to it
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> indirectX();

  /// @brief Indirect Y addressing mode: adds Y to the zero page pointer then
  /// reads a 16 bit address from that location
  /// @return The 16 bit effective address of the operand and if the page has
  /// been crossed
  std::pair<uint16_t, bool> indirectY();

  // -------- Instructions --------

  /// @brief Loads a memory value into the accumulator
  /// @param addr The address to read the operand from
  void lda(uint16_t addr);
};