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

QString getCurrentTimestampYaPayFormat()
{
    QDateTime current           = QDateTime::currentDateTime();
    QString   formattedDateTime = current.toString("yyyy-MM-ddTHH:mm:ss");
    int       offsetSeconds     = current.offsetFromUtc();
    QString   offset            = QString::asprintf("%+.2d:%.2d", offsetSeconds / 3600, (offsetSeconds % 3600) / 60);
    formattedDateTime += offset;
    return formattedDateTime;
}
