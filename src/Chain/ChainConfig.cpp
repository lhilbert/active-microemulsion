//
// Created by tommaso on 30/08/18.
//

#include <algorithm>
#include "ChainConfig.h"

std::istream &operator>>(std::istream &str, ChainConfig &data)
{
    std::string line;
    if (std::getline(str, line))
    {
        data.getLogger().logMsg(INFO, "ChainConfig::operator>> : New chain config read: %s", line.data());
        ChainConfig::removeCharFromString(line, ' ');
        std::stringstream iss(line);
        std::string tmpString, propertiesString, startPosString, stepsString;
        if (!(std::getline(iss, propertiesString, ':')
              && std::getline(iss, startPosString, ':')
              && std::getline(iss, stepsString, ':')
        ))
        {
            // One operation failed.
            // So set the state on the main stream
            // to indicate failure.
            str.setstate(std::ios::failbit);
            return str;
        }
        // Parse the chain properties
        ChainConfig::parseChainProperties(data, propertiesString);
        if (data.chainProperties.size() < 3)
        {
            str.setstate(std::ios::failbit);
            return str;
        }
        // Now parse the positions/displacements
        ChainConfig::parseStartPosition(data, startPosString);
        ChainConfig::parseSteps(data, stepsString);
        if (data.chainLength <= 1)
        {
            str.setstate(std::ios::failbit);
        }
    }
    return str;
}

ChainConfig &ChainConfig::parseChainProperties(ChainConfig &tmp, std::string &inputString)
{
    std::string tmpString;
    std::stringstream stringstream(inputString);
    while (getline(stringstream, tmpString, ','))
    {
        std::stringstream ssTmp(tmpString);
        std::string key, valueStr;
        if (!(getline(ssTmp, key, '=')
              && getline(ssTmp, valueStr, '=')))
        {
            tmp.getLogger().logMsg(DEBUG, "ChainConfig::parseChainProperties : Ignoring entry %s", tmpString.data());
            continue;
        }
        auto value = static_cast<unsigned char>(std::stoi(valueStr));
        tmp.chainProperties[key] = value;
    }
    return tmp;
}

ChainConfig &ChainConfig::parseStartPosition(ChainConfig &tmp, std::string &inputString)
{
    std::string xStr, yStr;
    removeCharFromString(inputString, '(');
    removeCharFromString(inputString, ')');
    std::stringstream stringstream(inputString);
    if (getline(stringstream, xStr, ',')
        && getline(stringstream, yStr, ','))
    {
        tmp.startCol = std::stoi(xStr);
        tmp.startRow = std::stoi(yStr);
    }
    else
    {
        tmp.startCol = 0;
        tmp.startRow = 0;
    }
    tmp.chainLength = 1;
    tmp.steps.clear();
    return tmp;
}

ChainConfig &ChainConfig::parseSteps(ChainConfig &tmp, std::string &inputString)
{
    std::string tmpString;
    std::stringstream stringstream(inputString);
    while (getline(stringstream, tmpString, ')'))
    {
        std::stringstream stepStream(tmpString);
        std::string multiplierStr, xDisplacementStr, yDisplacementStr;
        if (!(getline(stepStream, multiplierStr, '(')
              && getline(stepStream, xDisplacementStr, ',')
              && getline(stepStream, yDisplacementStr, ',')
        ))
        {
            continue;
        }
        int multiplier = 1;
        if (!multiplierStr.empty())
        {
            multiplier = std::stoi(multiplierStr);
        }
        if (xDisplacementStr == "+" || xDisplacementStr == "-")
        {
            xDisplacementStr.append("1");
        }
        if (yDisplacementStr == "+" || yDisplacementStr == "-" )
        {
            yDisplacementStr.append("1");
        }
        auto xDisplacement = static_cast<signed char>(std::stoi(xDisplacementStr));
        auto yDisplacement = static_cast<signed char>(std::stoi(yDisplacementStr));
        Displacement displacement(xDisplacement, yDisplacement);
        
        for (int i = 0; i < multiplier; ++i)
        {
            tmp.steps.push_back(displacement);
            ++tmp.chainLength;
        }
    }
    return tmp;
}

void ChainConfig::removeCharFromString(std::string &line, char filter)
{
    line.erase(remove(line.begin(), line.end(), filter), line.end()); // remove all spaces
}

const std::map<std::string, unsigned char> &ChainConfig::getChainProperties() const
{
    return chainProperties;
}

int ChainConfig::getStartCol() const
{
    return startCol;
}

int ChainConfig::getStartRow() const
{
    return startRow;
}

const std::vector<Displacement> &ChainConfig::getSteps() const
{
    return steps;
}

unsigned int ChainConfig::getChainLength() const
{
    return chainLength;
}

bool ChainConfig::isActive()
{
    return chainProperties["Active"] != 0;
}

bool ChainConfig::isTranscribable()
{
    return chainProperties["Transcribable"] != 0;
}

bool ChainConfig::isInhibited()
{
    return chainProperties["Inhibited"] != 0;
    
}

ChainConfig::ChainConfig(Logger &logger) : logger(logger),
                                           startCol(0),
                                           startRow(0),
                                           chainLength(0)
{
    chainProperties["Active"] = 0;
    chainProperties["Transcribable"] = 0;
    chainProperties["Inhibited"] = 0;
}

Logger &ChainConfig::getLogger() const
{
    return logger;
}
