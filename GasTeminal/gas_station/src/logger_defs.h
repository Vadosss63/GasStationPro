#pragma once

#include <QString>
#include <cstdint>

enum class LogLevel : uint8_t
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};

inline QString logLevelToString(LogLevel level)
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
