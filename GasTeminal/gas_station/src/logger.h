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
#include <QObject>
#include <QString>
#include <memory>

#include "filesystemutilities.h"
#include "ipstorage.h"
#include "logger_defs.h"

struct LoggerSettings
{
    QString  directory{"."};
    QString  filePrefix{};
    qint64   maxFileSize{1};
    qint64   maxFileNumber{1};
    LogLevel logLevel{LogLevel::DEBUG};
};

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(LoggerSettings settings);
    ~Logger() = default;

    Logger(const Logger&)            = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&)      = delete;

    void writeLog(LogLevel logLevel, const QString& message, const QString& funcName = "");
    void setLogLevel(LogLevel logLevel);

private slots:
    void handleSetLogLevelEvent(const QByteArray& receivedData);

private:
    void    cleanupDirIfRequired();
    QString getNewFilePath();
    bool    tryToOpenLatestLogFile();
    void    changeLogFile();
    bool    isLogStreamValid();

    LoggerSettings              settings;
    std::unique_ptr<FileWriter> logFileStream{};

    IPStorageReader logLvlFilter{loggerLvlStorage};

    static constexpr auto fileNameTempl = "%1%2_%3.log";
};
