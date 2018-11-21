//
// Created by tommaso on 10/08/18.
//

#ifndef ACTIVE_MICROEMULSION_CELLDATA_H
#define ACTIVE_MICROEMULSION_CELLDATA_H

#include <functional>
#include <set>
#include <cassert>
#include "../Utils/BitwiseOperations.h"

const unsigned char MAX_CROSSING_CHAINS = 2;
//todo: make sure to include some check to avoid swaps that cause this max to exceed

typedef unsigned char ChemicalProperties;
typedef enum
{
    SPECIES_BIT = 0, ACTIVE_BIT = 1
} ChemicalPropertiesBitMask;
typedef enum
{
    CHROMATIN = 0, RBP = 1
} ChemicalSpecies;
typedef enum
{
    NOT_ACTIVE = 0, ACTIVE = 1
} Activity;

typedef unsigned char Flags;
typedef enum
{
    TRANSCRIBABLE_BIT = 0, TRANSCRIPTION_INHIBITION_BIT = 1
} FlagsBitMask;
typedef enum
{
    NOT_TRANSCRIBABLE = 0, TRANSCRIBABLE = 1
} Transcribability;
typedef enum
{
    TRANSCRIPTION_INHIBITED = 0, TRANSCRIPTION_POSSIBLE = 1
} TranscriptionInhibition;

typedef unsigned short ChainId;
typedef struct ChainProperties
{
    // Note: chainId=0 CANNOT be used!
    ChainId chainId; //todo check how many chains we expect to have.
    unsigned short position; // Position of cell within the chain
    unsigned short chainLength; // chainLength==0 is the criterion for no chain
    ChainProperties() : chainId(0), position(0), chainLength(0)
    {};
} ChainProperties;

typedef unsigned short RnaCounter;

class CellData
{
public:
    ChemicalProperties chemicalProperties;
    Flags flags;
    RnaCounter rnaContent;
    ChainProperties chainProperties[MAX_CROSSING_CHAINS];

public:
    CellData() : chemicalProperties(0), flags(0), rnaContent(0)
    {};
    
    inline ChemicalProperties getChemicalProperties() const
    {
        return chemicalProperties;
    }
    
    static inline ChemicalSpecies getChemicalSpecies(ChemicalProperties chemicalProperties)
    {
        return static_cast<ChemicalSpecies>((chemicalProperties >> SPECIES_BIT) & 1U);
    }
    
    inline ChemicalSpecies getChemicalSpecies() const
    {
        return getChemicalSpecies(getChemicalProperties());
    }
    
    static inline bool isChromatin(ChemicalProperties chemicalProperties)
    {
        return getChemicalSpecies(chemicalProperties) == CHROMATIN;
    }
    
    inline bool isChromatin() const
    {
        return isChromatin(getChemicalProperties());
    }
    
    static inline bool isRBP(ChemicalProperties chemicalProperties)
    {
        return getChemicalSpecies(chemicalProperties) == RBP;
    }
    
    inline bool isRBP() const
    {
        return isRBP(getChemicalProperties());
    }
    
    static inline bool isActive(ChemicalProperties chemicalProperties)
    {
        return ((chemicalProperties >> ACTIVE_BIT) & 1U) == ACTIVE;
    }
    
    inline bool isActive() const
    {
        return isActive(getChemicalProperties());
    }
    
    inline bool isActiveChromatin() const
    {
        return isActiveChromatin(getChemicalProperties());
    }
    
    static inline bool isActiveChromatin(ChemicalProperties chemicalProperties)
    {
        return isChromatin(chemicalProperties) && isActive(chemicalProperties);
    }
    
    inline bool isInactiveChromatin() const
    {
        return isInactiveChromatin(getChemicalProperties());
    }
    
    static inline bool isInactiveChromatin(ChemicalProperties chemicalProperties)
    {
        return isChromatin(chemicalProperties) && !isActive(chemicalProperties);
    }
    
    inline bool isActiveRBP() const
    {
        return isActiveRBP(getChemicalProperties());
    }
    
    static inline bool isActiveRBP(ChemicalProperties chemicalProperties)
    {
        return isRBP(chemicalProperties) && isActive(chemicalProperties);
    }
    
    static inline bool isInactiveRBP(ChemicalProperties chemicalProperties)
    {
        return isRBP(chemicalProperties) && !isActive(chemicalProperties);
    }
    
    void setChemicalSpecies(ChemicalSpecies species);
    
    inline RnaCounter getRnaContent() const
    {
        return rnaContent;
    }
    
    inline void incrementRnaContent(RnaCounter amount = 1)
    {
        //todo: Here include an anti-overflow check!
        rnaContent += amount;
        assert(rnaContent >= 0);
    }
    
    void setActivity(Activity activity);
    
    inline void decrementRnaContent(RnaCounter amount = 1)
    {
        //todo: Here include an anti-underflow check!
        rnaContent -= amount;
        assert(rnaContent >= 0);
    }
    
    void setChemicalProperties(ChemicalSpecies species, Activity activity);
    
    inline Flags getFlags() const
    {
        return flags;
    }
    
    inline bool isTranscribable() const
    {
        return ((getFlags() >> TRANSCRIBABLE_BIT) & 1U) == TRANSCRIBABLE;
    }
    
    inline bool isTranscriptionInhibited() const
    {
        return ((getFlags() >> TRANSCRIPTION_INHIBITION_BIT) & 1U) == TRANSCRIPTION_INHIBITED;
    }
    
    static ChemicalProperties chemicalPropertiesOf(ChemicalSpecies species, Activity activity);
    
    void setFlags(Transcribability transcribability, TranscriptionInhibition inhibition);
    
    void setTranscribability(Transcribability transcribability);
    
    void setTranscriptionInhibition(TranscriptionInhibition inhibition);
    
    static Flags flagsOf(Transcribability transcribability, TranscriptionInhibition inhibition);
    
    std::set<ChainId> chainsCellBelongsTo();

private:
    static void setActivity(ChemicalProperties &chemicalProperties, Activity activity);
    
    static void setChemicalSpecies(ChemicalProperties &chemicalProperties, ChemicalSpecies species);
    
    static void setChemicalProperties(ChemicalProperties &chemicalProperties, ChemicalSpecies species,
                                      Activity activity);
    
    static void setTranscribability(Flags &flags, Transcribability transcribability);
    
    static void setTranscriptionInhibition(Flags &flags, TranscriptionInhibition inhibition);
    
    static void setFlags(Flags &flags, Transcribability transcribability, TranscriptionInhibition inhibition);
};

#endif //ACTIVE_MICROEMULSION_CELLDATA_H
