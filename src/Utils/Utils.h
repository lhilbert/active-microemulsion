//
// Created by tommaso on 20/11/18.
//

#ifndef ACTIVE_MICROEMULSION_UTILS_H
#define ACTIVE_MICROEMULSION_UTILS_H


class Utils
{
public:
    inline static bool getBit(unsigned char bitfield, unsigned char bit)
    {
        return static_cast<bool>((bitfield >> bit) & 1U);
    }
    
    inline static void setBit(unsigned char &bitfield, unsigned char bit, unsigned char value)
    {
        // Set the SPECIES_BIT to the species value
        bitfield ^= (-value ^ bitfield) & (1U << bit);
    }
};


#endif //ACTIVE_MICROEMULSION_UTILS_H
