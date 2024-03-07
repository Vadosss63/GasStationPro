#include <iostream>
#include <string>

#include "logger.h"
#include "logging.h"

namespace
{
const char* logLevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:
            return "DBG";
        case LogLevel::INFO:
            return "INF";
        case LogLevel::WARNING:
            return "WRN";
        case LogLevel::ERROR:
            return "ERR";
        default:
            return "UNKNOWN";
    }
}
}

void printLog(LogLevel logLevel, const QString& msg, const QString& funcName)
{
    std::cout << logLevelToString(logLevel) << "/" << funcName.toStdString() << " " << msg.toStdString() << std::endl;
}
