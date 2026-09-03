#pragma once

#include <cstdint>

// Forward declaration, cpu doesnt need the full Bus def
class Bus;

class Cpu {
public:
  enum StatusFlag : uint8_t {
    Carry = 1 << 0,     // ADC produces a carry
    Zero = 1 << 1,      // Set when value is equal to zero
    Interrupt = 1 << 2, // Set when you want to disable interrupts
    Decimal = 1 << 3,   // On NES this is disabled so no touchy
    Break = 1 << 4,     // Set when pushed by BRK or PHP
    Unused = 1 << 5,    // Pushed as 1
    Overflow = 1 << 6,  // Set when signed result is invalid (overflow)
    Negative = 1 << 7,  // Set when the result 7th bit is 1 (negative)
  };

  explicit Cpu(Bus& bus);

  bool getFlag(StatusFlag flag) const;
  void setFlag(StatusFlag flag, bool value);

private:
  Bus& bus;

  // Registers
  uint8_t a = 0;      // Logic
  uint8_t x = 0;      // Loop counters
  uint8_t y = 0;      // Loop counters
  uint16_t pc = 0;    // Program counter
  uint8_t sp = 0;     // Stack pointer
  uint8_t status = 0; // Holds all flags
};