//
// Created by tommaso on 20/11/18.
//

#include "CellData.h"

void CellData::setChemicalSpecies(ChemicalSpecies species)
{
    Utils::setBit(chemicalProperties, SPECIES_BIT, species);
}
void CellData::setChemicalSpecies(ChemicalProperties chemicalProperties, ChemicalSpecies species)
{
    Utils::setBit(chemicalProperties, SPECIES_BIT, species);
}

void CellData::setActivity(Activity activity)
{
    Utils::setBit(chemicalProperties, ACTIVE_BIT, activity);
}
void CellData::setActivity(ChemicalProperties chemicalProperties, Activity activity)
{
    Utils::setBit(chemicalProperties, ACTIVE_BIT, activity);
}

void CellData::setChemicalProperties(ChemicalSpecies species, Activity activity)
{
    setChemicalSpecies(species);
    setActivity(activity);
}

void CellData::setChemicalProperties(ChemicalProperties chemicalProperties, ChemicalSpecies species, Activity activity)
{
    setChemicalSpecies(chemicalProperties, species);
    setActivity(chemicalProperties, activity);
}

ChemicalProperties CellData::chemicalPropertiesOf(ChemicalSpecies species, Activity activity)
{
    ChemicalProperties chemicalProperties = 0;
    setChemicalProperties(chemicalProperties, species, activity);
    return chemicalProperties;
}

void CellData::setFlags(Transcribability transcribability, TranscriptionInhibition inhibition)
{
    setTranscribability(transcribability);
    setTranscriptionInhibition(inhibition);
}
void CellData::setFlags(Flags flags, Transcribability transcribability, TranscriptionInhibition inhibition)
{
    setTranscribability(flags, transcribability);
    setTranscriptionInhibition(flags, inhibition);
}

void CellData::setTranscribability(Transcribability transcribability)
{
    Utils::setBit(flags, TRANSCRIBABLE_BIT, transcribability);
}

void CellData::setTranscriptionInhibition(TranscriptionInhibition inhibition)
{
    Utils::setBit(flags, TRANSCRIPTION_INHIBITION_BIT, inhibition);
}

void CellData::setTranscribability(Flags flags, Transcribability transcribability)
{
    Utils::setBit(flags, TRANSCRIBABLE_BIT, transcribability);
}

void CellData::setTranscriptionInhibition(Flags flags, TranscriptionInhibition inhibition)
{
    Utils::setBit(flags, TRANSCRIPTION_INHIBITION_BIT, inhibition);
}

Flags CellData::flagsOf(Transcribability transcribability, TranscriptionInhibition inhibition)
{
    Flags flags = 0;
    setFlags(flags, transcribability, inhibition);
    return flags;
}

std::set<ChainId> CellData::chainsCellBelongsTo()
{
    auto chainIdSet = std::set<ChainId>();
    for (unsigned char k = 0; k < MAX_CROSSING_CHAINS; ++k)
    {
        ChainProperties &chain = chainProperties[k];
        unsigned int chainLength = chain.chainLength;
        if (chainLength > 0)
        {
            chainIdSet.insert(chain.chainId);
        }
    }
    return chainIdSet;
}
