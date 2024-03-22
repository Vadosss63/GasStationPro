#include "azsbutton.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "logging.h"

std::optional<AzsButton> readAzsButton(const QString& jsonText)
{
    const QByteArray    jsonData = QByteArray::fromStdString(jsonText.toStdString());
    const QJsonDocument document = QJsonDocument::fromJson(jsonData);

    if (document.isNull())
    {
        LOG_ERROR("Failed to parse JSON!");
        return std::nullopt;
    }

    const QJsonObject object = document.object();

    if (object.isEmpty() || !object.contains("id_azs") || !object.contains("value") || !object.contains("button"))
    {
        LOG_ERROR("Missing or invalid field(s)!");
        return std::nullopt;
    }
    AzsButton azsButton{};
    azsButton.idAzs  = object["id_azs"].toInt();
    azsButton.value  = object["value"].toInt();
    azsButton.button = object["button"].toInt();
    return {azsButton};
}
