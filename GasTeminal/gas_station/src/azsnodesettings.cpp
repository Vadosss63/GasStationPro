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

#include "azsnodesettings.h"

#include <QSettings>

namespace
{
constexpr auto appName   = "Gas_Station";
constexpr auto groupName = "parms";

constexpr auto gasTypeTemplate       = "gasType%1";
constexpr auto priceCashTemplate     = "priceCash%1";
constexpr auto priceCashlessTemplate = "priceCashless%1";
constexpr auto lockFuelValueTemplate = "lockFuelValue%1";

}

void writeAzsNodeSettings(const AzsNodeSettings& azsNodes)
{
    QSettings settings(appName);
    settings.beginGroup(groupName);
    for (int nodeId = 0; nodeId < maxAzsNodeCount; ++nodeId)
    {
        settings.setValue(QString(gasTypeTemplate).arg(nodeId), azsNodes.nodes[nodeId].gasType);
        settings.setValue(QString(priceCashTemplate).arg(nodeId), azsNodes.nodes[nodeId].priceCash);
        settings.setValue(QString(priceCashlessTemplate).arg(nodeId), azsNodes.nodes[nodeId].priceCashless);
        settings.setValue(QString(lockFuelValueTemplate).arg(nodeId), azsNodes.nodes[nodeId].lockFuelValue);
    }
    settings.endGroup();
}

AzsNodeSettings readAzsNodeSettings()
{
    AzsNodeSettings azsNodes;

    QSettings settings(appName);
    settings.beginGroup(groupName);

    for (int nodeId = 0; nodeId < maxAzsNodeCount; ++nodeId)
    {
        azsNodes.nodes[nodeId].gasType       = settings.value(QString(gasTypeTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].priceCash     = settings.value(QString(priceCashTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].priceCashless = settings.value(QString(priceCashlessTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].lockFuelValue = settings.value(QString(lockFuelValueTemplate).arg(nodeId)).toInt();
    }

    settings.endGroup();

    return azsNodes;
}
