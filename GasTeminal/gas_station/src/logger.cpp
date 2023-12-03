#include "logger.h"

#include <iostream>

void printLogInf(const QString& log)
{
    std::cout << log.toStdString() << std::endl;
}

void printLogInf(const QByteArray& data)
{
    QString print;
    foreach(auto b, data)
    {
        print.append("0x" + QString::number(static_cast<uint8_t>(b), 16) + " ");
    }
    printLogInf(print);
}

void printLogErr(const QString& log)
{
    std::cerr << log.toStdString() << std::endl;
}
