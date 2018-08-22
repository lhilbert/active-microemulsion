//
// Created by tommaso on 08/08/18.
//

#include "PgmWriter.h"

PgmWriter::PgmWriter(Logger &logger, int W, int H, std::string outputFile, std::string channelName,
                     ChemicalProperties signalMatcher) : logger(logger),
                                                         width(W), height(H), depth(1),
                                                         outputFileName(outputFile),
                                                         channelName(channelName),
                                                         signalChemicalMatcher(signalMatcher),
                                                         pgm(nullptr), counter(0)
{
    logger.logMsg(INFO, "Initializing PGM writer for channel %s", channelName.data());
    advanceSeries();
}

void PgmWriter::setData(const CellData **newData)
{
    data = newData;
}

void PgmWriter::write()
{
    logger.logMsg(PRODUCTION, "Writing PGM file #%d, channel %s (%s)",
            getCounter(),
            channelName.data(),
            getOutputFileFullNameCstring());
    __write();
}

void PgmWriter::write(double t)
{
    logger.logEvent(PRODUCTION, t, "Writing PGM file #%d, channel %s (%s)",
                    getCounter(),
                    channelName.data(),
                    getOutputFileFullNameCstring());
    __write();
}

void PgmWriter::advanceSeries()
{
    if (pgm != nullptr)
    {
        std::fclose(pgm);
        ++counter; // If we just initialized the class, there is no need to increment counter!
    }
    outputFileFullName = outputFileName + "_" + std::to_string(counter) + ".pgm";
    pgm = std::fopen(outputFileFullName.data(), "wb");
    
}

PgmWriter::~PgmWriter()
{
    std::fclose(pgm);
}

unsigned int PgmWriter::getCounter()
{
    return counter;
}

const char *PgmWriter::getOutputFileFullNameCstring()
{
    return outputFileFullName.data();
}

void PgmWriter::__write()
{
    fprintf(pgm, "P2\n");
    fprintf(pgm, "# Channel: %s\n", channelName.data());
    fprintf(pgm, "# 0 - NoSignal\n");
    fprintf(pgm, "# 1 - Signal\n");
    fprintf(pgm, "%d %d\n", width, height);
    fprintf(pgm, "%d\n", depth);
    
    // Create buffer for one row of *.pgm picture, including spaces and trailing newline.
    char *buffer = new char[2 * width + 1];
    // Now write one row of data into the buffer, then write it to file
    for (int j = height; j > 0; j--)
    {
        for (int i = 0; i <= 2 * width - 1; i += 2)
        {
            ChemicalProperties chemicalProperties = data[(i / 2) + 1][j].chemicalProperties;
            char character = 48; // ASCII '0' char
            if (chemicalProperties == signalChemicalMatcher)
            {
                character = 49; // ASCII '1' char
            }
            buffer[i] = character;
            buffer[i + 1] = ' ';
        }
        buffer[2 * width - 1] = '\n';
        // terminating the buffer as a proper c-string
        buffer[2 * width] = '\0';
        
        fprintf(pgm, "%s", buffer);
    }
    delete[] buffer;
}

