//
// Created by tommaso on 30/08/18.
//

#ifndef ACTIVE_MICROEMULSION_CHAINCONFIG_H
#define ACTIVE_MICROEMULSION_CHAINCONFIG_H


typedef struct Displacement
{
    signed char x;
    signed char y;
    
    Displacement() : x(0), y(0)
    {};
    
    Displacement(signed char x, signed char y) : x(x), y(y)
    {};
} Displacement;

#include <map>
#include <vector>
#include <sstream>
#include "Logger.h"

class ChainConfig
{
private:
    Logger &logger;
    std::map<std::string, unsigned char> chainProperties;
    int startCol, startRow;
    std::vector<Displacement> steps;
    unsigned int chainLength;

public:
    ChainConfig(Logger &logger);
    
    const std::map<std::string, unsigned char> &getChainProperties() const;
    
    int getStartCol() const;
    
    int getStartRow() const;
    
    const std::vector<Displacement> &getSteps() const;
    
    unsigned int getChainLength() const;
    
    Logger &getLogger() const;
    
    bool isChromatin() { return true;};
    
    bool isActive();
    
    bool isTranscribable();
    
    bool isInhibited();

private:
    friend std::istream &operator>>(std::istream &str, ChainConfig &data);
    
    static ChainConfig &parseChainProperties(ChainConfig &tmp, std::string &inputString);
    
    static ChainConfig &parseStartPosition(ChainConfig &tmp, std::string &inputString);
    
    static ChainConfig &parseSteps(ChainConfig &tmp, std::string &inputString);
    
    static void removeCharFromString(std::string &line, char filter);
};

#endif //ACTIVE_MICROEMULSION_CHAINCONFIG_H
