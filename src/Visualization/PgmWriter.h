//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_PGMWRITER_H
#define ACTIVE_MICROEMULSION_PGMWRITER_H

#include <iostream>
#include <cstdio>
#include "../Cell/CellData.h"
#include "../Logger/Logger.h"

class PgmWriter
{
private:
    Logger &logger;
    const int width, height;
    const unsigned int depth;
    std::string outputFileName;
    std::string channelName;
    unsigned char (*signalConverter)(const CellData &cellData);
    const CellData **data;
    std::FILE *pgm;
    unsigned int counter;
    std::string outputFileFullName;
    std::string outputFileFullNameExtra;
    
public:
    /*
     * Initializes a binary (0=noSignal, 1=Signal) PgmWriter instance with given:
     * - Width, Height
     * - Output file name base
     * - Channel name to be reported in the legend
     * - The chemical property to be matched for signal
     */
    PgmWriter(Logger &logger, int W, int H, std::string outputFile, std::string channelName,
              unsigned char (*signalConverter)(const CellData &cellData));
    ~PgmWriter();
    // Data pointer should usually be done just once.
    void setData(const CellData **newData);
    // Write data to pgm file (logging without simulation time)
    void write(bool isExtraSnapshot=false);
    // Write data to pgm file (logging with simulation time)
    void write(double t, bool isExtraSnapshot=false);
    // Series should be advanced after write, if necessary
    void advanceSeries();
    unsigned int getCounter();
    const char * getOutputFileFullNameCstring();
private:
    // Actual writing process, without logging
    void __write(bool isExtraSnapshot=false);
};


#endif //ACTIVE_MICROEMULSION_PGMWRITER_H
