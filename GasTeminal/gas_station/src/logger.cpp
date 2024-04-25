/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "logger.h"

#include <assert.h>
#include <iostream>

#include "utilities.h"

namespace
{
QString formatMessage(LogLevel logLevel, const QString& message, const QString& funcName)
{
    static constexpr auto messageTempl = "%1 %2/%3: %4\n";
    return QString(messageTempl).arg(getCurrentTimestamp(), logLevelToString(logLevel), funcName, message.trimmed());
}
}

Logger::Logger(LoggerSettings settings) : settings(std::move(settings))
{
    assert(settings.maxFileSize && "Maximum log file size could not be equal to zero");
    assert(settings.maxFileNumber && "Maximum log files could not be equal to zero");
    if (!tryToOpenLatestLogFile())
    {
        changeLogFile();
    }
    connect(&logLvlFilter, SIGNAL(receivedData(QByteArray)), this, SLOT(handleSetLogLevelEvent(QByteArray)));
}

void Logger::writeLog(LogLevel logLevel, const QString& message, const QString& funcName)
{
    if (logLevel < settings.logLevel)
    {
        return;
    }

    if (!isLogStreamValid())
    {
        return;
    }

    const QString     formatedMsg = formatMessage(logLevel, message, funcName);
    const std::string dataToWrite{formatedMsg.toUtf8().constData()};

    const qint64            writedBytes = logFileStream->writeData(dataToWrite.c_str(), dataToWrite.size());
    static constexpr qint64 writeError{-1};
    if (writedBytes == writeError)
    {
        std::cerr << logFileStream->errorString().toStdString() << std::endl;
    }
}

bool Logger::isLogStreamValid()
{
    if (!logFileStream)
    {
        std::cerr << "Log file is unreachable" << std::endl;
        return false;
    }

    if (logFileStream->size() >= settings.maxFileSize)
    {
        if (changeLogFile(); !logFileStream)
        {
            std::cerr << "Failed to change a log file" << std::endl;
            return false;
        }
    }

    if (!logFileStream->isOpen())
    {
        std::cerr << logFileStream->errorString().toStdString() << std::endl;
        return false;
    }

    return true;
}

void Logger::changeLogFile()
{
    if (!createDirIfNeeded(settings.directory))
    {
        std::cerr << "Failed to create directory for log files" << std::endl;
        return;
    }

    logFileStream = FileWriter::openFile(getNewFilePath(), QIODevice::WriteOnly);

    cleanupDirIfRequired();
}

bool Logger::tryToOpenLatestLogFile()
{
    if (!isDirectoryExist(settings.directory))
    {
        return false;
    }

    logFileStream = FileWriter::openLatestFileInDir(settings.directory, QIODevice::WriteOnly | QIODevice::Append);
    return logFileStream ? isLogStreamValid() : false;
}

void Logger::cleanupDirIfRequired()
{
    if (const int numberOfFiles = getNumberOfFilesInDir(settings.directory); numberOfFiles >= settings.maxFileNumber)
    {
        removeOlderFilesInDir(settings.directory, settings.maxFileNumber);
    }
}

QString Logger::getNewFilePath()
{
    return QString(fileNameTempl).arg(settings.directory).arg(settings.filePrefix).arg(getCurrentTimestamp());
}

void Logger::setLogLevel(LogLevel logLevel)
{
    settings.logLevel = logLevel;
}

void Logger::handleSetLogLevelEvent(const QByteArray& receivedData)
{
    static_assert((LogLevel::ERROR > LogLevel::WARNING) && (LogLevel::WARNING > LogLevel::INFO) &&
                  (LogLevel::INFO > LogLevel::DEBUG) && "Logger severity ordering was changed");
    if (receivedData.size() > static_cast<int>(sizeof(LogLevel)))
    {
        std::cerr << "Failed to convert received data to logger severity level, wrong data size was received: "
                  << receivedData.size() << std::endl;
        return;
    }

    bool isConverted{};
    auto newLogLevel = receivedData.toUShort(&isConverted);
    if (!isConverted || (newLogLevel > static_cast<ushort>(LogLevel::ERROR)))
    {
        std::cerr << "Failed to converted received data to logger severity level" << std::endl;
        return;
    }
    std::cout << "Loglvl changed to: " << logLevelToString(static_cast<LogLevel>(newLogLevel)).toStdString()
              << std::endl;
    setLogLevel(static_cast<LogLevel>(newLogLevel));
}
