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

  /// @brief Get accumulator register value
  /// @return The accumulator register
  uint8_t getA() const { return a; }

  /// @brief Sets the accumulator
  /// @param value The value to set as the new accumulator
  void setA(uint8_t value) { a = value; }

  /// @brief  Get the X register value
  /// @return The X register
  uint8_t getX() const { return x; }

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

  /// @brief Sets zero and negative flags depending on the value
  /// @param value The value to set the flags against
  void updateZeroNegativeFlags(uint8_t value);

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
    bool canCrossPage = true;
  };

  /// @brief Lookup table of all the possible opcode values
  std::array<Instruction, 256> table;

  /// @brief Populates table with all implemented opcodes
  void buildTable();

  // ============================================================
  // Addressing Modes
  // ============================================================

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

  /// @brief Zero page Y addressing mode: adds the Y register to the address
  /// provided by the operand
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> zeroPageY();

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

  /// @brief Implied addressing mode: used by instructions that have no address
  /// operand
  /// @return A placeholder address
  std::pair<uint16_t, bool> implied();

  /// @brief Accumulator addressing mode: operates directly on the accumulator
  /// @return A placeholder address, ignored by the instruction (e.g. ASL A)
  std::pair<uint16_t, bool> accumulator();

  // ============================================================
  // Access Instructions
  // ============================================================

  /// @brief Loads a memory value into the accumulator
  /// @param addr The address containing the operand
  void lda(uint16_t addr);

  /// @brief Stores the accumulators value into memory
  /// @param addr The address to write the accumulators value to
  void sta(uint16_t addr);

  /// @brief Loads a memory value into the X register
  /// @param addr The address containing the operand
  void ldx(uint16_t addr);

  /// @brief Stores the X registers value into memory
  /// @param addr The address to write the X registers value to
  void stx(uint16_t addr);

  /// @brief Loads a memory value into the Y register
  /// @param addr The address containing the operand
  void ldy(uint16_t addr);

  /// @brief Stores the Y registers value into memory
  /// @param addr The address to write the Y registers value to
  void sty(uint16_t addr);

  // ============================================================
  // Transfer Instructions
  // ============================================================

  /// @brief Copies the accumulator value to the X register
  /// @param unused placeholder parameter
  void tax(uint16_t /* unused */);

  /// @brief Copies the X register value to the accumulator
  /// @param unused placeholder parameter
  void txa(uint16_t /* unused */);

  /// @brief Copies the accumulator value to the Y register
  /// @param unused placeholder parameter
  void tay(uint16_t /* unused */);

  /// @brief Copies the Y register value to the accumulator
  /// @param unused placeholder parameter
  void tya(uint16_t /* unused */);

  // ============================================================
  // Arithmetic Instructions
  // ============================================================

  /// @brief Add the carry flag and a memory value to the accumulator
  /// @param addr The address of the value to add to the accumulator
  void adc(uint16_t addr);

  /// @brief Subtract the memory value and the NOT of the carry flag from the
  /// accumulator.
  /// @param addr The address of the value to subtract from the accumulator
  void sbc(uint16_t addr);

  /// @brief Adds one to a memory location
  /// @param addr The address of memory to add one
  void inc(uint16_t addr);

  /// @brief Subtracts one from a memory location
  /// @param addr The address of memory to decrease by one.
  void dec(uint16_t addr);

  /// @brief Adds one to a X register
  /// @param unused placeholder parameter
  void inx(uint16_t /* unused */);

  /// @brief Subtract one rom the X register
  /// @param unused placeholder parameter
  void dex(uint16_t /* unused */);

  /// @brief Adds one to a Y register
  /// @param unused placeholder parameter
  void iny(uint16_t /* unused */);

  /// @brief Subtract one rom the Y register
  /// @param unused placeholder parameter
  void dey(uint16_t /* unused */);

  // ============================================================
  // Shift Instructions
  // ============================================================

  /// @brief Shift all bits to the left by one position
  /// @param addr The address of the value to shift
  void asl(uint16_t addr);

  /// @brief Shift all bits of the accumulator to the left by one position
  /// @param addr placeholder parameter
  void asl_a(uint16_t /* unused */);

  /// @brief Shifts all bits to the right by one position
  /// @param addr The address of the value to shift
  void lsr(uint16_t addr);

  /// @brief Shifts all bits of the accumulator to the right by one position
  /// @param addr The address of the value to shift
  void lsr_a(uint16_t /* unused */);

  /// @brief Rotates all the bits to the left by one position through the carry flag
  /// @param addr The address of the value to rotate
  void rol(uint16_t addr);

  /// @brief Rotates all the bits of the accumulator to the left by one position through the carry
  /// flag
  /// @param addr placeholder parameter
  void rol_a(uint16_t /* unused */);

  /// @brief Rotates all the bits to the right by one position through the carry flag
  /// @param addr The address of the value to rotate
  void ror(uint16_t addr);

  /// @brief Rotates all the bits of the accumulator to the right by one position through the carry
  /// flag
  /// @param addr placeholder parameter
  void ror_a(uint16_t /* unused */);

  // ============================================================
  // Bitwise Instructions
  // ============================================================

  /// @brief ANDs a memory value and the accumulator
  /// @param addr The address of the value
  void and_a(uint16_t addr);

  /// @brief Inclusive ORs a memory value and the accumulator
  /// @param addr The address of the value
  void ora(uint16_t addr);

  /// @brief Exclube ORs a memory value and the accumulaor
  /// @param addr The address of the value
  void eor(uint16_t addr);

  /// @brief BIT modifies flags but does not change memory or registers.
  /// @param addr The address of the value
  void bit(uint16_t addr);

  // ============================================================
  // Compare Instructions
  // ============================================================

  /// @brief Compare A to a memory value and set appropriate flags, does not touch any registers
  /// @param addr The address of the value
  void cmp(uint16_t addr);

  /// @brief Compare X to a memory value and set appropriate flags, does not touch any registers
  /// @param addr The address of the value
  void cpx(uint16_t addr);

  /// @brief Compare Y to a memory value and set appropriate flags, does not touch any registers
  /// @param addr The address of the value
  void cpy(uint16_t addr);
};
