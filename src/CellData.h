//
// Created by tommaso on 10/08/18.
//

#ifndef ACTIVE_MICROEMULSION_CELLDATA_H
#define ACTIVE_MICROEMULSION_CELLDATA_H

const unsigned char MAX_CROSSING_CHAINS = 2;
//todo: make sure to include some check to avoid swaps that cause this max to exceed

typedef unsigned char ChemicalProperties;
typedef enum {SPECIES_BIT=0, ACTIVE_BIT=1} ChemicalPropertiesBitMask;
typedef enum {CHROMATIN=0, RBP=1} ChemicalSpecies;
typedef enum {NOT_ACTIVE=0, ACTIVE=1} Activity;

typedef unsigned char Flags;
typedef enum {TRANSCRIBABLE_BIT=0, TRANSCRIPTION_INHIBITION_BIT=1} FlagsBitMask;
typedef enum {NOT_TRANSCRIBABLE=0, TRANSCRIBABLE=1} Transcribability;
typedef enum {TRANSCRIPTION_INHIBITED=0, TRANSCRIPTION_POSSIBLE=1} TranscriptionInhibition;

typedef unsigned short ChainId;
typedef struct ChainProperties
{
    // Note: chainId=0 CANNOT be used!
    ChainId chainId; //todo check how many chains we expect to have.
    unsigned short position; // Position of cell within the chain
    unsigned short chainLength; // chainLength==0 is the criterion for no chain
    ChainProperties() : chainId(0), position(0), chainLength(0) {};
} ChainProperties;

typedef unsigned short RnaCounter;

typedef struct CellData
{
    ChemicalProperties chemicalProperties;
    Flags flags;
    RnaCounter rnaContent;
    ChainProperties chainProperties[MAX_CROSSING_CHAINS];
    CellData() : chemicalProperties(0), flags(0), rnaContent(0) {};
} CellData;

#endif //ACTIVE_MICROEMULSION_CELLDATA_H
