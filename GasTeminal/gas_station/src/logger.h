#pragma once

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
