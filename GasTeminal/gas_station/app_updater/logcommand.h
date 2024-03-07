#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <optional>

#include "logging.h"

struct UpdateCommand
{
    int     idAzs{};
    QString url{};

    static std::optional<UpdateCommand> readCommand(const QString& jsonText)
    {
        const QByteArray jsonData = QByteArray::fromStdString(jsonText.toStdString());
        QJsonDocument    document = QJsonDocument::fromJson(jsonData);

        if (document.isNull())
        {
            LOG_ERROR("Failed to parse JSON!");
            return std::nullopt;
        }

        QJsonObject object = document.object();

        if (object.isEmpty() || !object.contains("id_azs") || !object.contains("url"))
        {
            LOG_ERROR("Missing or invalid field(s)!");
            return std::nullopt;
        }

        return UpdateCommand{.idAzs = object["id_azs"].toInt(), .url = object["url"].toString()};
    }
};
