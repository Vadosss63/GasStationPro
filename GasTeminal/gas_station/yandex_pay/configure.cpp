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

#include "configure.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "filesystemutilities.h"
#include "logging.h"

namespace
{
constexpr auto idToken       = "Id";
constexpr auto tokenToken    = "Token";
constexpr auto hostToken     = "Host";
constexpr auto comPortToken  = "ComPort";
constexpr auto baudRateToken = "BaudRate";
}

std::optional<Configure> readConfigure(const QString& fileName)
{
    std::unique_ptr<QIODevice> file = openFile(fileName, QIODevice::ReadOnly | QIODevice::Text);
    if (!file)
    {
        return std::nullopt;
    }
    QByteArray    jsonData = file->readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    file->close();

    if (document.isNull())
    {
        LOG_ERROR("Failed to parse JSON!");
        return std::nullopt;
    }

    QJsonObject json = document.object();

    const QList<QString> requiredFields = {idToken, tokenToken, hostToken, comPortToken, baudRateToken};

    for (const QString& field : requiredFields)
    {
        if (!json.contains(field))
        {
            LOG_ERROR("Missing or invalid field in settings: " + field);
            return std::nullopt;
        }
    }
    Configure configure{.id       = json[idToken].toString(),
                        .token    = json[tokenToken].toString(),
                        .host     = json[hostToken].toString(),
                        .comPort  = json[comPortToken].toString(),
                        .baudRate = json[baudRateToken].toInt()};

    return configure;
}
