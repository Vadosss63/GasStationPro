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

#include <QJsonDocument>
#include <QJsonObject>

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
