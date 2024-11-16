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

#include "azsstatisticshandler.h"

#include "responsedata.h"
#include "utilities.h"
#include "volumeconvertor.h"

namespace
{
AzsStatistics getResetAzsStatistics(const AzsReport& azsReport)
{
    const auto countNode = azsReport.countNode;

    const auto& rec = azsReport.rec;

    auto dailyCash     = static_cast<float>(rec.dailyCashSum + rec.dailyCoinsSum);
    auto dailyCashless = static_cast<float>(rec.dailyCashlessSum);
    auto dailyOnline   = static_cast<float>(rec.dailyOnlineSum);

    AzsStatistics azsStatistics;
    azsStatistics.time            = currentSecsSinceEpoch();
    azsStatistics.date            = currentDateTime();
    azsStatistics.dailyCash       = dailyCash;
    azsStatistics.dailyCashless   = dailyCashless;
    azsStatistics.dailyOnline     = dailyOnline;
    azsStatistics.dailyLitersCol1 = convertIntToLiter(rec.azsNodes[0].daily);

    if (countNode == 2)
    {
        azsStatistics.dailyLitersCol2 = convertIntToLiter(rec.azsNodes[1].daily);
    }
    return azsStatistics;
}

AzsStatistics getFuelArrival1Statistics(const AzsButton& azsButton, bool isFirstColumn)
{
    AzsStatistics azsStatistics;
    azsStatistics.time = currentSecsSinceEpoch();
    azsStatistics.date = currentDateTime();
    if (isFirstColumn)
    {
        azsStatistics.fuelArrivalCol1 = azsButton.value;
    }
    else
    {
        azsStatistics.fuelArrivalCol2 = azsButton.value;
    }
    return azsStatistics;
}
} // namespace

std::optional<AzsStatistics> handleAzsStatistics(const AzsButton& azsButton, const AzsReport& azsReport)
{
    using State = ResponseData::Command;

    switch (azsButton.button)
    {
        case State::setFuelArrival1:
            return getFuelArrival1Statistics(azsButton, true);
        case State::setFuelArrival2:
            return getFuelArrival1Statistics(azsButton, false);
        case State::resetCounters:
            return getResetAzsStatistics(azsReport);
    }

    return std::nullopt;
}
