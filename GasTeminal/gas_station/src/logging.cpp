#include "logging.h"

#include <iostream>
#include <string>

#include "logger.h"

void printLog(LogLevel logLevel, const QString& msg, const QString& funcName)
{
    constexpr const char* logDirectory = "./azs_logs/";
    constexpr const char* fileNamePrefix{"azs_log_file"};
    constexpr qint64      maxLogFileSize{202400};
    constexpr qint64      maxLogFiles{4};
    static Logger         logger{logDirectory, fileNamePrefix, maxLogFileSize, maxLogFiles, LogLevel::DEBUG};
    logger.writeLog(logLevel, msg, funcName);
}

void printLogInf(const QString& log)
{
    std::cout << log.toStdString() << std::endl;
}

void printLogInf(const QByteArray& data)
{
    printLogInf(convertToString(data));
}

void printLogErr(const QString& log)
{
    std::cerr << log.toStdString() << std::endl;
}

QString convertToString(const QByteArray& data)
{
    QString res{};
    for (const auto byte : data)
    {
        res.append("0x" + QString::number(static_cast<uint8_t>(byte), 16) + " ");
    }
    return res;
}
