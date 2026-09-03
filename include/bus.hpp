#pragma once

#include <cstdint>
#include <array>

class Bus
{
public:
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr) const;

private:
    std::array<uint8_t, 65536> memory{};
};