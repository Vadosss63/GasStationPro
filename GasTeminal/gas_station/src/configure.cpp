#include "configure.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "filesystemutilities.h"
#include "logging.h"

namespace
{
constexpr auto idToken              = "Id";
constexpr auto nameToken            = "Name";
constexpr auto addressToken         = "Address";
constexpr auto tokenToken           = "Token";
constexpr auto hostToken            = "Host";
constexpr auto phoneOfSupportToken  = "PhoneOfSupport";
constexpr auto activeBtn2Token      = "ActiveBtn2";
constexpr auto token                = "ComPort";
constexpr auto comPortToken         = "BaudRate";
constexpr auto showSecondPriceToken = "ShowSecondPrice";
}

std::optional<Configure> readConfigure(const QString& fileName)
{
    std::unique_ptr<QIODevice> file = openFile(fileName, QIODevice::ReadOnly | QIODevice::Text);
    if (!file->isOpen())
    {
        LOG_ERROR("Failed to open file!");
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

    const QList<QString> requiredFields = {idToken,
                                           nameToken,
                                           addressToken,
                                           tokenToken,
                                           hostToken,
                                           phoneOfSupportToken,
                                           activeBtn2Token,
                                           token,
                                           comPortToken,
                                           showSecondPriceToken};

    for (const QString& field : requiredFields)
    {
        if (!json.contains(field))
        {
            LOG_ERROR("Missing or invalid field in settings: " + field);
            return std::nullopt;
        }
    }
    Configure configure{.id              = json[idToken].toString(),
                        .name            = json[nameToken].toString(),
                        .address         = json[addressToken].toString(),
                        .token           = json[tokenToken].toString(),
                        .host            = json[hostToken].toString(),
                        .phoneOfSupport  = json[phoneOfSupportToken].toString(),
                        .activeBtn2      = json[activeBtn2Token].toInt(),
                        .comPort         = json[token].toString(),
                        .baudRate        = json[comPortToken].toInt(),
                        .showSecondPrice = json[showSecondPriceToken].toInt()};

    return configure;
}
