//
// Created by tommaso on 03/08/18.
//

#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <stdexcept>
#include "Timing.h"
#include "Logger.h"

static const char *DEBUG_STR[] = {
        FOREACH_DEBUG(GENERATE_STRING)
};


Logger::Logger()
{
    setDebugLevel(INFO);
    setOutputFolder(".");
    setLogFileName("sim.log");
    setStartTime();
}

void Logger::setStartTime()
{
    LOGGER_START_TIME = Timing::getCurrentTimeMillis();
}

void Logger::setOutputFolder(const char *outputFolder)
{
    strcpy(LOG_FILE_FOLDER, outputFolder);
    sprintf(LOG_FILE_FULL_PATH, "%s/%s", LOG_FILE_FOLDER, LOG_FILE_NAME);
}

void Logger::setLogFileName(const char *fileName)
{
    strcpy(LOG_FILE_NAME, fileName);
    sprintf(LOG_FILE_FULL_PATH, "%s/%s", LOG_FILE_FOLDER, LOG_FILE_NAME);
}

void Logger::setDebugLevel(DebugLevel debugLevel)
{
    DEBUG_LEVEL = debugLevel;
}

DebugLevel Logger::getDebugLevel()
{
    return DEBUG_LEVEL;
}

const char * Logger::getDebugLevelStr()
{
    return DEBUG_STR[DEBUG_LEVEL];
}

void Logger::openLogFile()
{
    if (strlen(LOG_FILE_FULL_PATH) == 0)
    {
        sprintf(LOG_FILE_FULL_PATH, "%s/%s", LOG_FILE_FOLDER, LOG_FILE_NAME);
    }
    LOG_FILE = std::fopen(LOG_FILE_FULL_PATH, "w");
    if (LOG_FILE == nullptr)
    {
        char buf[2048];
        sprintf(buf, "Cannot open file %s", LOG_FILE_FULL_PATH);
        throw std::invalid_argument(buf);
    }
}

void Logger::logRawString(char const *fmt, ...)
{
    // Newline at the end of the message is included.
    double timestamp = Timing::getTimeSpentSeconds(LOGGER_START_TIME, Timing::getCurrentTimeMillis());
    va_list args;
    va_start(args,fmt);
    printf("[%06.3f] ", timestamp);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    va_start(args,fmt);
    fprintf(LOG_FILE, "[%06.3f] ", timestamp);
    vfprintf(LOG_FILE, fmt, args);
    fprintf(LOG_FILE, "\n");
    va_end(args);
}

void Logger::logEvent(DebugLevel eventDebugLevel, double t, char const *fmt, ...)
{
    // Newline at the end of the message is included.
    // If this trace is too low level for current debug level, skip it.
    if (eventDebugLevel < DEBUG_LEVEL)
        return;
    //
    double timestamp = Timing::getTimeSpentSeconds(LOGGER_START_TIME, Timing::getCurrentTimeMillis());
    va_list args;
    va_start(args,fmt);
    printf("[%06.3f] [%012.9f] %s: ", timestamp, t, DEBUG_STR[eventDebugLevel]);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    va_start(args,fmt);
    fprintf(LOG_FILE, "[%06.3f] [%012.9f] %s: ", timestamp, t, DEBUG_STR[eventDebugLevel]);
    vfprintf(LOG_FILE, fmt, args);
    fprintf(LOG_FILE, "\n");
    va_end(args);
}

void Logger::logMsg(DebugLevel eventDebugLevel, char const *fmt, ...)
{
    // Newline at the end of the message is included.
    // If this trace is too low level for current debug level, skip it.
    if (eventDebugLevel < DEBUG_LEVEL)
        return;
    //
    double timestamp = Timing::getTimeSpentSeconds(LOGGER_START_TIME, Timing::getCurrentTimeMillis());
    va_list args;
    va_start(args,fmt);
    printf("[%06.3f] %s ", timestamp, DEBUG_STR[eventDebugLevel]);
    printf("---> ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    va_start(args,fmt);
    fprintf(LOG_FILE, "[%06.3f] %s ", timestamp, DEBUG_STR[eventDebugLevel]);
    fprintf(LOG_FILE, "---> ");
    vfprintf(LOG_FILE, fmt, args);
    fprintf(LOG_FILE, "\n");
    va_end(args);
}

void Logger::closeLogFile()
{
    std::fclose(LOG_FILE);
}

Logger::~Logger()
{
    logMsg(getDebugLevel(), "Closing logger.");
    closeLogFile();
}

void Logger::flush()
{
    std::fflush(stdout);
    std::fflush(LOG_FILE);
}

void Logger::logArgv(const int argc, const char **argv)
{
    std::string cmdline("Invocation cmd: ");
    for (int i = 0; i < argc - 1; ++i)
    {
        cmdline += argv[i];
        cmdline += " ";
    }
    cmdline += argv[argc - 1];
    logRawString(cmdline.data());
}

// todo: find some way to log variables nicely
//void Logger::logVars(DebugLevel eventDebugLevel, char const * fmt, ...)
//{
//    // Newline at the end of the message is included.
//    // If this trace is too low level for current debug level, skip it.
//    if (eventDebugLevel < DEBUG_LEVEL)
//        return;
//    //
//    double timestamp = Timing::getTimeSpentSeconds(LOGGER_START_TIME, Timing::getCurrentTimeMillis());
//    va_list args;
//    va_start(args,fmt);
//    printf("[%06.3f] %s ", timestamp, DEBUG_STR[eventDebugLevel]);
//    printf("---> ");
//    vprintf(fmt, args);
//    printf("\n");
//    va_end(args);
//    va_start(args,fmt);
//    fprintf(LOG_FILE, "[%06.3f] %s ", timestamp, DEBUG_STR[eventDebugLevel]);
//    fprintf(LOG_FILE, "---> ");
//    vfprintf(LOG_FILE, fmt, args);
//    fprintf(LOG_FILE, "\n");
//    va_end(args);
//}
