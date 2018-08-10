//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_PGMWRITER_H
#define ACTIVE_MICROEMULSION_PGMWRITER_H

#include <iostream>
#include <cstdio>

class PgmWriter
{
public:
    const int width, height;
    const unsigned int depth;
    std::string outputFileName;
    const unsigned int **data;
private:
    std::FILE *pgm;
    unsigned int counter;
    std::string outputFileFullName;
    
public:
    PgmWriter(int W, int H, unsigned int depth,
            std::string outputFile);
    ~PgmWriter();
    // Data pointer should usually be done just once.
    void setData(const unsigned int **newData);
    // Write data to pgm file
    void write();
    // Series should be advanced after write, if necessary
    void advanceSeries();
    unsigned int getCounter();
    const char * getOutputFileFullNameCstring();
};


#endif //ACTIVE_MICROEMULSION_PGMWRITER_H
