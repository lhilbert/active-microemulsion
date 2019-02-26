//
// Created by tommaso on 08/08/18.
//

#include "PgmWriter.h"
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

PgmWriter::PgmWriter(Logger &logger, int W, int H, std::string outputFile, std::string channelName,
                     unsigned char (*signalConverter)(const CellData &cellData))
        : logger(logger),
          width(W), height(H), depth(255),
          outputFileName(outputFile),
          channelName(channelName),
          signalConverter(signalConverter),
          pgm(nullptr), counter(0)
{
    logger.logMsg(INFO, "Initializing PGM writer for channel %s", channelName.data());
    advanceSeries();
    outputFileFullNameExtra = outputFileName + "_EXTRA.pgm";
}

void PgmWriter::setData(const CellData **newData)
{
    data = newData;
}

void PgmWriter::write(double t, bool isExtraSnapshot)
{
    std::string pgmId = std::to_string(getCounter());
    const char *writtenFname = getOutputFileFullNameCstring(t);
    if (isExtraSnapshot)
    {
        pgmId = "EXTRA";
        writtenFname = outputFileFullNameExtra.data();
    }
    logger.logEvent(PRODUCTION, t, "Writing PGM file #%s, channel %s (%s)",
                    pgmId.data(),
                    channelName.data(),
                    writtenFname);
    __write(t, isExtraSnapshot);
}

void PgmWriter::advanceSeries()
{
    if (pgm != nullptr)
    {
        ++counter; // If we just initialized the class, there is no need to increment counter!
    }
}

PgmWriter::~PgmWriter()
{
//    std::fclose(pgm);
}

unsigned int PgmWriter::getCounter()
{
    return counter;
}

const std::string PgmWriter::setOutputFileFullName(double t)
{
    std::stringstream tStream;
    tStream << std::fixed << std::setprecision(2) << t;
    outputFileFullName = outputFileName + "_" + std::to_string(counter) + "_" + tStream.str() + ".pgm";
    return outputFileFullName;
}

const char *PgmWriter::getOutputFileFullNameCstring(double t)
{
    setOutputFileFullName(t);
    return outputFileFullName.data();
}

void PgmWriter::__write(double t, bool isExtraSnapshot)
{
    if (isExtraSnapshot)
    {
        pgm = std::fopen(outputFileFullNameExtra.data(), "wb");
    }
    else
    {
        pgm = std::fopen(getOutputFileFullNameCstring(t), "wb");
    }
    fprintf(pgm, "P2\n");
    fprintf(pgm, "# Channel: %s\n", channelName.data());
    fprintf(pgm, "# 0 - NoSignal\n");
    fprintf(pgm, "# 1 - Signal\n");
    fprintf(pgm, "%d %d\n", width, height);
    fprintf(pgm, "%d\n", depth);
    
    // Create buffer for one row of *.pgm picture, including spaces and trailing newline.
    char *buffer = new char[4 * width + 1]; // The 4 multiplier is because we need to accommodate up to 255 values
    // Now write one row of data into the buffer, then write it to file
    for (int row = height; row > 0; row--)
    {
        buffer[0] = '\0'; // Reset the buffer for each row
        for (int column = 1; column <= width; ++column)
        {
            const CellData cellData = data[row][column];
            char intensity[8];
            sprintf(intensity, "%d", signalConverter(cellData));
            strcat(buffer, intensity);
            strcat(buffer, " ");
        }
        // append newline (\0 termination is included in strcat)
        strcat(buffer, "\n");
        
        fprintf(pgm, "%s", buffer);
    }
    std::fclose(pgm);
    delete[] buffer;
}

