//
// Created by tommaso on 08/08/18.
//

#include "PgmWriter.h"

PgmWriter::PgmWriter(const int W, const int H, const unsigned int depth,
                     char const *outputFile) : width(W), height(H), depth(depth),
                                               outputFileName(outputFile), pgm(nullptr), counter(0)
{
    outputFileFullName = new char[1024];
    advanceSeries();
}

void PgmWriter::setData(const unsigned int **newData)
{
    data = newData;
}

void PgmWriter::write()
{
    fprintf(pgm, "P2\n");
    fprintf(pgm, "# 0 - SpeciesA\n");
    fprintf(pgm, "# 1 - SpeciesB\n");
    fprintf(pgm, "%d %d\n", width, height);
    fprintf(pgm, "%d\n", depth);
    
    // Create buffer for one row of *.pgm picture, including spaces and trailing newline.
    char *buffer = new char[2 * width + 1];
    // Now write one row of data into the buffer, then write it to file
    for (int j = height - 1; j >= 0; j--)
    {
        for (int i = 0; i <= 2 * width - 1; i += 2)
        {
            int value = data[i / 2][j];
            char character = static_cast<char>(48 + value); // DANGER: this works only with single digit values!
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

void PgmWriter::advanceSeries()
{
    if (pgm != nullptr)
    {
        std::fclose(pgm);
        ++counter; // If we just initialized the class, there is no need to increment counter!
    }
    sprintf(outputFileFullName, "%s_%d.pgm", outputFileName, counter);
    pgm = std::fopen(outputFileFullName, "wb");
    
}

PgmWriter::~PgmWriter()
{
    std::fclose(pgm);
}

unsigned int PgmWriter::getCounter()
{
    return counter;
}

