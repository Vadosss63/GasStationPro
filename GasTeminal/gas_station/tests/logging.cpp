#include "logging.h"

#include <iostream>
#include <string>

void printLog(LogLevel, const QString& msg, const QString&)
{
    printLogInf(msg);
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
