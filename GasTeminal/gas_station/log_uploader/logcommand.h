#pragma once
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
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <optional>

#include "logging.h"

struct LogCommand
{
    int idAzs{};
    int download{};

    static std::optional<LogCommand> readCommand(const QString& jsonText)
    {
        const QByteArray jsonData = QByteArray::fromStdString(jsonText.toStdString());
        QJsonDocument    document = QJsonDocument::fromJson(jsonData);

        if (document.isNull())
        {
            LOG_ERROR("Failed to parse JSON!");
            return std::nullopt;
        }

        QJsonObject object = document.object();

        if (object.isEmpty() || !object.contains("id_azs") || !object.contains("download"))
        {
            LOG_ERROR("Missing or invalid field(s)!");
            return std::nullopt;
        }

        return LogCommand{.idAzs = object["id_azs"].toInt(), .download = object["download"].toInt()};
    }
};
