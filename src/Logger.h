//
// Created by tommaso on 03/08/18.
//

#ifndef ACTIVE_MICROEMULSION_LOGGER_H
#define ACTIVE_MICROEMULSION_LOGGER_H

#include <iostream>
#include <fstream>

// This allows for automatically getting strings of debug levels (see https://stackoverflow.com/a/10966395 )
// NOTE: order is important for correctly managing incremental levels of debug.
#define FOREACH_DEBUG(DLEVEL) \
    DLEVEL(DEBUG) \
    DLEVEL(INFO) \
    DLEVEL(PRODUCTION) \
    DLEVEL(WARNING) \
    DLEVEL(ERROR) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum DebugLevel
{
    FOREACH_DEBUG(GENERATE_ENUM)
} DebugLevel;

class Logger
{
private:
    long LOGGER_START_TIME = 0;
    DebugLevel DEBUG_LEVEL = INFO; // Default level
    char LOG_FILE_FOLDER[512] = "./";
    char LOG_FILE_NAME[256];
    char LOG_FILE_FULL_PATH[1024] = "";
    std::FILE* LOG_FILE;
    
public:
    Logger();
    ~Logger();
    void setStartTime();
    void setOutputFolder(const char * outputFolder);
    void setLogFileName(const char * fileName);
    void setDebugLevel(DebugLevel debugLevel);
    DebugLevel getDebugLevel();
    const char * getDebugLevelStr();
    void openLogFile();
    void logRawString(char const *fmt, ...);
    void logEvent(DebugLevel eventDebugLevel, double t, char const *fmt, ...);
    void logMsg(DebugLevel eventDebugLevel, char const *fmt, ...);
    void closeLogFile();
};


#endif //ACTIVE_MICROEMULSION_LOGGER_H
