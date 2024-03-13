#include <iostream>
#include <string>

#include "logger.h"
#include "logging.h"

void printLog(LogLevel logLevel, const QString& msg, const QString& funcName)
{
    std::cout << logLevelToString(logLevel).toStdString() << "/" << funcName.toStdString() << " " << msg.toStdString()
              << std::endl;
}
