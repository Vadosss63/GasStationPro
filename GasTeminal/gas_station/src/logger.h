#pragma once
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
#include <QIODevice>
#include <QString>
#include <memory>

#include "logger_defs.h"

class Logger
{
public:
    Logger(const QString& logDir,
           const QString& fileNamePrefix,
           qint64         maxLogFileSize,
           qint64         maxLogFiles,
           LogLevel       initLogLevel);
    ~Logger() = default;

    Logger(const Logger&)            = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&)      = delete;

    void writeLog(LogLevel logLevel, const QString& message, const QString& funcName = "");

private:
    void changeLogFile();
    bool isLogFileValid();
    bool tryToOpenLatestLogFile();

    QString                    logDirectory;
    QString                    baseFileName;
    qint64                     maxFileSize;
    qint64                     maxFiles;
    LogLevel                   initialLogLevel;
    std::unique_ptr<QIODevice> logFileStream{};
};
