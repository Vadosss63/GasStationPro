#include "logger.h"

#include <iostream>

void printLog(const QString& log)
{
    std::cout << log.toStdString() << std::endl;
}

void printLog(const QByteArray& data)
{
    QString print;
    foreach(auto b, data)
    {
        print.append("0x" + QString::number(static_cast<uint8_t>(b), 16) + " ");
    }
    printLog(print);
}
