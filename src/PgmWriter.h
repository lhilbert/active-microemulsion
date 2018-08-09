//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_PGMWRITER_H
#define ACTIVE_MICROEMULSION_PGMWRITER_H


#include <cstdio>

class PgmWriter
{
public:
    const int width, height;
    const unsigned int depth;
    char const *outputFileName;
    const unsigned int **data;
private:
    std::FILE *pgm;
    unsigned int counter;
    char * outputFileFullName;
    
public:
    PgmWriter(int W, int H, unsigned int depth,
            char const *outputFile);
    ~PgmWriter();
    // Data pointer should usually be done just once.
    void setData(const unsigned int **newData);
    // Write data to pgm file
    void write();
    // Series should be advanced after write, if necessary
    void advanceSeries();
    unsigned int getCounter();
};


#endif //ACTIVE_MICROEMULSION_PGMWRITER_H
