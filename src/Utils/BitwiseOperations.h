//
// Created by tommaso on 20/11/18.
//

#ifndef ACTIVE_MICROEMULSION_UTILS_H
#define ACTIVE_MICROEMULSION_UTILS_H


class BitwiseOperations
{
public:
    static bool getBit(unsigned char bitfield, unsigned char bit);
    
    static void setBit(unsigned char &bitfield, unsigned char bit, unsigned char value);
};


#endif //ACTIVE_MICROEMULSION_UTILS_H
