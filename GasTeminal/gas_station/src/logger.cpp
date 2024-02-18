#include "logger.h"

#include <iostream>

#include "filesystemutilities.h"
#include "utilities.h"

namespace
{
QString logLevelToString(LogLevel level)
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

Logger::Logger(const QString& logDir,
               const QString& fileNamePrefix,
               qint64         maxLogFileSize,
               qint64         maxLogFiles,
               LogLevel       initLogLevel)
    : logDirectory(logDir)
    , baseFileName(fileNamePrefix)
    , maxFileSize(maxLogFileSize)
    , maxFiles(maxLogFiles)
    , initialLogLevel(initLogLevel)
{
    assert(maxLogFileSize && "Maximum log file size could not be equal to zero");
    assert(maxLogFiles && "Maximum log files could not be equal to zero");
    changeLogFile();
}

void Logger::writeLog(LogLevel logLevel, const QString& message, const QString& funcName)
{
    if (static_cast<uint8_t>(logLevel) < static_cast<uint8_t>(initialLogLevel))
    {
        return;
    }

    if (!isLogFileValid())
    {
        return;
    }

    const QString formatedMsg =
        QString("%1 %2/%3: %4\n").arg(getCurrentTimestamp(), logLevelToString(logLevel), funcName, message);
    const std::string dataToWrite{formatedMsg.toStdString()};

    if (logFileStream->write(dataToWrite.c_str()) == -1)
    {
        std::cerr << "Failed to write to log file" << std::endl;
        return;
    }
}

void Logger::changeLogFile()
{
    if (!createDir(logDirectory))
    {
        std::cerr << "Failed to create directory for log files" << std::endl;
        return;
    }

    const QString newLogFile = QString("%1%2_%3.log").arg(logDirectory).arg(baseFileName).arg(getCurrentTimestamp());

    logFileStream = openFile(newLogFile, QIODevice::WriteOnly);

    if (const int numberOfFiles = getNumberOfFilesInDir(logDirectory); numberOfFiles >= maxFiles)
    {
        removeOlderFilesInDir(logDirectory, maxFiles);
    }
}

bool Logger::isLogFileValid()
{
    if (!logFileStream)
    {
        std::cerr << "Log file is unreachable" << std::endl;
        return false;
    }

    const qint64 fileSize = logFileStream->size();
    if (fileSize >= maxFileSize)
    {
        changeLogFile();
    }

    if (!logFileStream->isOpen())
    {
        std::cerr << "Failed to open log file" << std::endl;
        return false;
    }

    return true;
}
