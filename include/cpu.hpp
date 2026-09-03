#pragma once

#include <cstdint>
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
};