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

#include "azsstatistics.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "appsettings.h"
#include "filesystemutilities.h"
#include "logging.h"

namespace
{

constexpr auto idToken              = "id";
constexpr auto timeToken            = "time";
constexpr auto dateToken            = "date";
constexpr auto dailyCashToken       = "daily_cash";
constexpr auto dailyCashlessToken   = "daily_cashless";
constexpr auto dailyOnlineToken     = "daily_online";
constexpr auto dailyLitersCol1Token = "daily_liters_col1";
constexpr auto dailyLitersCol2Token = "daily_liters_col2";
constexpr auto fuelArrivalCol1Token = "fuel_arrival_col1";
constexpr auto fuelArrivalCol2Token = "fuel_arrival_col2";

} // namespace

QString AzsStatistics::getStatisticsJson() const
{
    const QJsonObject statistics = {{idToken, id},
                                    {timeToken, time},
                                    {dateToken, date},
                                    {dailyCashToken, dailyCash},
                                    {dailyCashlessToken, dailyCashless},
                                    {dailyOnlineToken, dailyOnline},
                                    {dailyLitersCol1Token, dailyLitersCol1},
                                    {dailyLitersCol2Token, dailyLitersCol2},
                                    {fuelArrivalCol1Token, fuelArrivalCol1},
                                    {fuelArrivalCol2Token, fuelArrivalCol2}};

    QString statisticsJson = QString::fromUtf8(QJsonDocument(statistics).toJson());
    return statisticsJson;
}

QStringList getListAzsStatisticsFiles()
{
    const QString folderPath = AppSettings::instance().getAzsStatsFolderName();

    const QRegExp     expr{"^\\d{10}\\.json$"};
    const QStringList allFiles{getDirectoryFileList(folderPath)};

    return allFiles.filter(expr);
}

void writeAzsStatisticsToFile(const AzsStatistics& statistics)
{
    const QString folderPath = AppSettings::instance().getAzsStatsFolderName();
    const QString filePath   = QString("%1%2.json").arg(folderPath).arg(statistics.time);

    if (!createDirIfNeeded(folderPath))
    {
        LOG_ERROR("Failed to create directory: " + folderPath);
        return;
    }

    std::unique_ptr<QIODevice> fileStats = openFile(filePath, QIODevice::WriteOnly);
    if (!fileStats)
    {
        LOG_ERROR("Failed to open file for writing statistics");
        return;
    }

    const std::string dataTowrite{statistics.getStatisticsJson().toStdString()};

    fileStats->write(dataTowrite.c_str());
    fileStats->close();

    LOG_INFO(QString("Statistics written to file: %1").arg(filePath));
}

std::optional<AzsStatistics> readAzsStatisticsFromFile(const QString& fileStatisticsPath)
{
    std::unique_ptr<QIODevice> fileStats = openFile(fileStatisticsPath, QIODevice::ReadOnly | QIODevice::Text);

    if (!fileStats)
    {
        LOG_ERROR("Can not open json file");
        return std::nullopt;
    }

    const QString jsonData = fileStats->readAll();
    fileStats->close();
    const QJsonDocument doc  = QJsonDocument::fromJson(jsonData.toUtf8());
    const QJsonObject   json = doc.object();

    const QStringList requiredFields = {idToken,
                                        timeToken,
                                        dateToken,
                                        dailyCashToken,
                                        dailyCashlessToken,
                                        dailyOnlineToken,
                                        dailyLitersCol1Token,
                                        dailyLitersCol2Token,
                                        fuelArrivalCol1Token,
                                        fuelArrivalCol2Token};
    for (const auto& field : requiredFields)
    {
        if (!json.contains(field))
        {
            LOG_ERROR("Missing or invalid field in settings: " + QString(field));
            return std::nullopt;
        }
    }
    AzsStatistics statistics{.id              = json[idToken].toInt(),
                             .time            = json[timeToken].toInt(),
                             .date            = json[dateToken].toString(),
                             .dailyCash       = static_cast<float>(json[dailyCashToken].toDouble()),
                             .dailyCashless   = static_cast<float>(json[dailyCashlessToken].toDouble()),
                             .dailyOnline     = static_cast<float>(json[dailyOnlineToken].toDouble()),
                             .dailyLitersCol1 = static_cast<float>(json[dailyLitersCol1Token].toDouble()),
                             .dailyLitersCol2 = static_cast<float>(json[dailyLitersCol2Token].toDouble()),
                             .fuelArrivalCol1 = static_cast<float>(json[fuelArrivalCol1Token].toDouble()),
                             .fuelArrivalCol2 = static_cast<float>(json[fuelArrivalCol2Token].toDouble())};
    return statistics;
}
