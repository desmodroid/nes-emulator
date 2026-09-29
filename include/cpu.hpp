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

  /// @brief Gets the PC value
  /// @return The PC register
  uint16_t getPC() const { return pc; }

  /// @brief Gets the SP value
  /// @return The SP register
  uint8_t getSP() const { return sp; }

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

  /// @brief Extra cycles if a branch has been taken and page has been crossed
  int extraCycles = 0;

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

  // -------- Stack helpers --------
  /// @brief Writes a byte to the stack and decrements the stack pointer
  /// @param byte The byte to push to the stack
  void push(uint8_t byte);

  /// @brief Increments the stack pointer and reads the byte from the stack
  uint8_t pull();

  /// @brief Computes the processor status with break and unused bits forced to 1.
  /// @return The status byte to push
  uint8_t statusForPush();

  static constexpr uint16_t StackBase = 0x0100;

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
  /// @return The 16 bit effective address of the operand and if the page has been crossed
  std::pair<uint16_t, bool> absolute();

  /// @brief Absolute X addressing mode: adds the X register to the address
  /// provided by the operand
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> absoluteX();

  /// @brief Absolute Y addressing mode: adds the Y register to the address
  /// provided by the operand
  /// @return The 16 bit effective address of the operand and if the page has been crossed
  std::pair<uint16_t, bool> absoluteY();

  /// @brief Indirect X addressing mode: reads a 16 bit address from a zero page
  /// pointer then adds Y to it
  /// @return The 16 bit effective address of the operand
  std::pair<uint16_t, bool> indirectX();

  /// @brief Indirect Y addressing mode: adds Y to the zero page pointer then
  /// reads a 16 bit address from that location
  /// @return The 16 bit effective address of the operand and if the page has been crossed
  std::pair<uint16_t, bool> indirectY();

  std::pair<uint16_t, bool> indirect();

  /// @brief Implied addressing mode: used by instructions that have no address
  /// operand
  /// @return A placeholder address
  std::pair<uint16_t, bool> implied();

  /// @brief Accumulator addressing mode: operates directly on the accumulator
  /// @return A placeholder address, ignored by the instruction (e.g. ASL A)
  std::pair<uint16_t, bool> accumulator();

  /// @brief Relative addressing mode: Specifies an 8 bit signed offset relative to the PC
  /// @return The 16 bit effective address of the operand. The page crossed bool is always false
  /// here as the branch instuctions handle that themselves
  std::pair<uint16_t, bool> relative();

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
  void tax(uint16_t /* unused */);

  /// @brief Copies the X register value to the accumulator
  void txa(uint16_t /* unused */);

  /// @brief Copies the accumulator value to the Y register
  void tay(uint16_t /* unused */);

  /// @brief Copies the Y register value to the accumulator
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
  void inx(uint16_t /* unused */);

  /// @brief Subtract one rom the X register
  void dex(uint16_t /* unused */);

  /// @brief Adds one to a Y register
  void iny(uint16_t /* unused */);

  /// @brief Subtract one rom the Y register
  void dey(uint16_t /* unused */);

  // ============================================================
  // Shift Instructions
  // ============================================================

  /// @brief Shift all bits to the left by one position
  /// @param addr The address of the value to shift
  void asl(uint16_t addr);

  /// @brief Shift all bits of the accumulator to the left by one position
  void asl_a(uint16_t /* unused */);

  /// @brief Shifts all bits to the right by one position
  /// @param addr The address of the value to shift
  void lsr(uint16_t addr);

  /// @brief Shifts all bits of the accumulator to the right by one position
  void lsr_a(uint16_t /* unused */);

  /// @brief Rotates all the bits to the left by one position through the carry flag
  /// @param addr The address of the value to rotate
  void rol(uint16_t addr);

  /// @brief Rotates all the bits of the accumulator to the left by one position through the carry
  /// flag
  void rol_a(uint16_t /* unused */);

  /// @brief Rotates all the bits to the right by one position through the carry flag
  /// @param addr The address of the value to rotate
  void ror(uint16_t addr);

  /// @brief Rotates all the bits of the accumulator to the right by one position through the carry
  /// flag
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

  // ============================================================
  // Branch Instructions
  // ============================================================

  /// @brief Helper function for all branch instructions
  /// If condition is true, jumps pc to addr and add cycles if needed
  /// @param condition The condition to determine to branch
  /// @param addr The 16 bit effective address of the operand
  void branch(bool condition, uint16_t addr);

  /// @brief Branches if carry is clear
  /// @param addr The 16 bit effective address of the operand
  void bcc(uint16_t addr);

  /// @brief Branches if carry is set
  /// @param addr The 16 bit effective address of the operand
  void bcs(uint16_t addr);

  /// @brief Branches if the zero flag is set
  /// @param addr The 16 bit effective address of the operand
  void beq(uint16_t addr);

  /// @brief Branches if the zero flag is clear
  /// @param addr The 16 bit effective address of the operand
  void bne(uint16_t addr);

  /// @brief Branches if the negative flag is clear
  /// @param addr The 16 bit effective address of the operand
  void bpl(uint16_t addr);

  /// @brief Branch is the negative flag is set
  /// @param addr The 16 bit effective address of the operand
  void bmi(uint16_t addr);

  /// @brief Branch if the overflow flag is clear
  /// @param addr The 16 bit effective address of the operand
  void bvc(uint16_t addr);

  /// @brief Branch if the overflow flag is set
  /// @param addr The 16 bit effective address of the operand
  void bvs(uint16_t addr);

  // ============================================================
  // Jump Instructions
  // ============================================================

  /// @brief Jumps PC to a new addr
  /// @param addr The 16 bit effective target address
  void jmp(uint16_t addr);

  /// @brief Pushes the return address onto the stack and jumps PC to addr
  /// @param addr The 16 bit effective target address
  void jsr(uint16_t addr);

  /// @brief Pulls the return address off of the stack and resumes at that instruction
  void rts(uint16_t /* unused */);

  /// @brief Pushes the return address and status onto the stack then
  /// jumps to the address stored at 0xFFFE
  void brk(uint16_t /* unusued */);

  /// @brief Pulls status and return address from the stack and resumes from there
  void rti(uint16_t /* unused */);

  // ============================================================
  // Stack Instructions
  // ============================================================

  /// @brief Pushes the A register onto the stack
  void pha(uint16_t /* unused */);

  /// @brief Pulls the A register from the stack and stores it
  void pla(uint16_t /* unused */);

  /// @brief Pushes the status flag and break flag to the stack
  void php(uint16_t /* unused */);

  /// @brief Pulls the processor status from the stack and stores it
  void plp(uint16_t /* unused */);

  /// @brief Copies the X register value to the stack pointer
  void txs(uint16_t /* unused */);

  /// @brief Copies the stack pointer to the X register
  void tsx(uint16_t /* unused */);

  // ============================================================
  // Flag Instructions
  // ============================================================

  /// @brief Clears the carry flag
  void clc(uint16_t /* unused */);

  /// @brief Sets the carry flag
  void sec(uint16_t /* unused */);

  /// @brief Clears the interrupt disable flag
  void cli(uint16_t /* unused */);

  /// @brief Sets the interrupt disable flag
  void sei(uint16_t /* unused */);

  /// @brief Clears the decimal flag
  void cld(uint16_t /* unused */);

  /// @brief Sets the decimal flag
  void sed(uint16_t /* unused */);

  /// @brief Clears the overflow flag
  void clv(uint16_t /* unused */);
};
