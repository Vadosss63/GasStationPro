#include "logger.h"

#include <assert.h>
#include <iostream>

#include "filesystemutilities.h"
#include "utilities.h"

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
    if (!tryToOpenLatestLogFile())
    {
        changeLogFile();
    }
}

void Logger::writeLog(LogLevel logLevel, const QString& message, const QString& funcName)
{
    if (logLevel < initialLogLevel)
    {
        return;
    }

    if (!isLogFileValid())
    {
        return;
    }

    const QString formatedMsg =
        QString("%1 %2/%3: %4\n").arg(getCurrentTimestamp(), logLevelToString(logLevel), funcName, message.trimmed());
    const std::string dataToWrite{formatedMsg.toUtf8().constData()};

    constexpr qint64 writeError{-1};
    auto writedBytes= logFileStream->write(dataToWrite.c_str());
    if (writedBytes == writeError)
    {
        std::cerr << logFileStream->errorString().toStdString() << std::endl;
        return;
    }
}

void Logger::changeLogFile()
{
    if (!createDirIfNeeded(logDirectory))
    {
        std::cerr << "Failed to create directory for log files" << std::endl;
        return;
    }

    const QString newLogFile = QString("%1%2_%3.log").arg(logDirectory).arg(baseFileName).arg(getCurrentTimestamp());

    logFileStream = openFile(newLogFile, QIODevice::WriteOnly);

    if (!logFileStream)
    {
        std::cerr << "Fail to open log file" << std::endl;
        return;
    }

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

    if (logFileStream->size() >= maxFileSize)
    {
        changeLogFile();
    }

    if (!logFileStream->isOpen())
    {
        std::cerr << logFileStream->errorString().toStdString() << std::endl;
        return false;
    }

    return true;
}

bool Logger::tryToOpenLatestLogFile()
{
    if (!isDirectoryExist(logDirectory))
    {
        return false;
    }

    logFileStream = tryToOpenLatestFileInDir(logDirectory, QIODevice::WriteOnly | QIODevice::Append);
    return logFileStream ? isLogFileValid() : false;
}
