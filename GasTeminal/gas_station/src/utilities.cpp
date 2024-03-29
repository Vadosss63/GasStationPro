#include "utilities.h"

#include <QDateTime>
#include <QString>

QString getCurrentTimestamp()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss");
}

int currentSecsSinceEpoch()
{
    return static_cast<int>(QDateTime::currentSecsSinceEpoch());
}

QString currentDateTime()
{
    return QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
}
