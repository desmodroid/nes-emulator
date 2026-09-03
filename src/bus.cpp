#include "bus.hpp"

void Bus::write(uint16_t addr, uint8_t data)
{
    memory[addr] = data;
}

uint8_t Bus::read(uint16_t addr) const
{
    return memory[addr];
}