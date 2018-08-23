//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_PGMWRITER_H
#define ACTIVE_MICROEMULSION_PGMWRITER_H

#include <iostream>
#include <cstdio>
#include "CellData.h"
#include "Logger.h"

class PgmWriter
{
private:
    Logger &logger;
    const int width, height;
    const unsigned int depth;
    std::string outputFileName;
    std::string channelName;
    bool (*signalMatcher)(ChemicalProperties chemicalProperties);
    const CellData **data;
    std::FILE *pgm;
    unsigned int counter;
    std::string outputFileFullName;
    
public:
    /*
     * Initializes a binary (0=noSignal, 1=Signal) PgmWriter instance with given:
     * - Width, Height
     * - Output file name base
     * - Channel name to be reported in the legenda
     * - The chemical property to be matched for signal
     */
    PgmWriter(Logger &logger, int W, int H, std::string outputFile, std::string channelName,
              bool (*signalMatcher)(ChemicalProperties chemicalProperties));
    ~PgmWriter();
    // Data pointer should usually be done just once.
    void setData(const CellData **newData);
    // Write data to pgm file (logging without simulation time)
    void write();
    // Write data to pgm file (logging with simulation time)
    void write(double t);
    // Series should be advanced after write, if necessary
    void advanceSeries();
    unsigned int getCounter();
    const char * getOutputFileFullNameCstring();
private:
    // Actual writing process, without logging
    void __write();
};


#endif //ACTIVE_MICROEMULSION_PGMWRITER_H
