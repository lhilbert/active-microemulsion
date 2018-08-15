//
// Created by tommaso on 10/08/18.
//

#ifndef ACTIVE_MICROEMULSION_CELLDATA_H
#define ACTIVE_MICROEMULSION_CELLDATA_H

const unsigned char MAX_CROSSING_CHAINS = 4;

typedef enum {SPECIES_BIT=0, TRANSCRIBABLE_BIT=1} SpeciesBitMask;
typedef enum {SPECIES_A=0, SPECIES_B=1} SpeciesBit;

typedef struct ChainProperties
{
    // Note: chainId=0 CANNOT be used!
    unsigned short chainId; //todo check how many chains we expect to have.
    unsigned int position; // Position of cell within the chain
    unsigned int chainLength; // chainLength==0 is the criterion for no chain
    ChainProperties() : chainId(0), position(0), chainLength(0)
    {};
} ChainProperties;

typedef struct CellData
{
    char chemicalSpecies;
    char flags;
    ChainProperties chainProperties[MAX_CROSSING_CHAINS];
} CellData;

#endif //ACTIVE_MICROEMULSION_CELLDATA_H
