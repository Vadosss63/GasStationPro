#pragma once

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
