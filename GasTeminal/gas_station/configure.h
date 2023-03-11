#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

struct Configure
{
    QString id             = "11111111";
    QString name           = "";
    QString address        = "";
    QString token          = "";
    QString host           = "http://t-azs.ru:8080";
    QString phoneOfSupport = "8(999)000-00-00";
    int     activeBtn2     = true;
    QString comPort        = "/dev/ttyS4";
    int     baudRate       = 115200;
};

bool readConfigure(const QString& fileName, Configure& settings);

#endif // CONFIGURE_H
