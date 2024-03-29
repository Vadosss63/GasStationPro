#pragma once

#include <QString>
#include <optional>

struct Configure
{
    QString id              = "11111111";
    QString name            = "";
    QString address         = "";
    QString token           = "";
    QString host            = "http://t-azs.ru:8080";
    QString phoneOfSupport  = "8(000)111-66-77";
    int     activeBtn2      = true;
    QString comPort         = "/dev/ttyS4";
    int     baudRate        = 115200;
    int     showSecondPrice = false;
};

std::optional<Configure> readConfigure(const QString& fileName);
