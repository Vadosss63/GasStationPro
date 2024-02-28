#include "utilities.h"

#include <QDateTime>
#include <QString>

#include "logging.h"

QString getCurrentTimestamp()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss");
}
