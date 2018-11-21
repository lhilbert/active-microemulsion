//
// Created by tommaso on 20/11/18.
//

#include "BitwiseOperations.h"

bool BitwiseOperations::getBit(unsigned char bitfield, unsigned char bit)
{
    return static_cast<bool>((bitfield >> bit) & 1U);
}

void BitwiseOperations::setBit(unsigned char &bitfield, unsigned char bit, unsigned char value)
{
    // Set the SPECIES_BIT to the species value
    bitfield ^= (-value ^ bitfield) & (1U << bit);
}
