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

#include "report.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "responsedata.h"
#include "volumeconvertor.h"

namespace
{

QString convertFromDouble(double val)
{
    return QString("%1").arg(val, 0, 'f', 2);
}
}

QString getJsonReport(const AzsReport& azsReport)
{
    const size_t countNode = azsReport.countNode;

    const ReceivedData&    rec     = azsReport.rec;
    const AzsNodeSettings& azsNode = azsReport.azsNode;

    QJsonObject mainInfo;
    mainInfo.insert("commonCash", static_cast<int>(rec.commonCashSum + rec.commonCoinsSum));
    mainInfo.insert("dailyCash", static_cast<int>(rec.dailyCashSum + rec.dailyCoinsSum));

    mainInfo.insert("commonCashless", static_cast<int>(rec.commonCashlessSum));
    mainInfo.insert("dailyCashless", static_cast<int>(rec.dailyCashlessSum));

    mainInfo.insert("commonOnline", static_cast<int>(rec.commonOnlineSum));
    mainInfo.insert("dailyOnline", static_cast<int>(rec.dailyOnlineSum));

    mainInfo.insert("version", azsReport.version);
    mainInfo.insert("isBlock", azsReport.isBlock);

    QJsonArray jsonAzsNodes;
    for (size_t i = 0; i < countNode; ++i)
    {
        QJsonObject jsonAzsNode;
        jsonAzsNode.insert("commonLiters", convertFromDouble(convertIntToLiter(rec.azsNodes[i].common)));
        jsonAzsNode.insert("dailyLiters", convertFromDouble(convertIntToLiter(rec.azsNodes[i].daily)));

        auto type = convertIntToGasType(azsNode.nodes[i].gasType);
        jsonAzsNode.insert("typeFuel", convertGasTypeToString(type));
        jsonAzsNode.insert("price", static_cast<int>(azsNode.nodes[i].priceCash));
        jsonAzsNode.insert("priceCashless", static_cast<int>(azsNode.nodes[i].priceCashless));
        jsonAzsNode.insert("lockFuelValue", static_cast<int>(azsNode.nodes[i].lockFuelValue));
        jsonAzsNode.insert("fuelArrival", 0);

        int fuelVolume = static_cast<int>(rec.azsNodes[i].fuelVolume * 1000);
        jsonAzsNode.insert("fuelVolume", QString::number(fuelVolume));
        jsonAzsNode.insert("fuelVolumePerc", convertFromDouble(rec.azsNodes[i].fuelVolumePerc));
        jsonAzsNode.insert("density", convertFromDouble(rec.azsNodes[i].density));
        jsonAzsNode.insert("averageTemperature", convertFromDouble(rec.azsNodes[i].averageTemperature));
        jsonAzsNodes.append(jsonAzsNode);
    }

    QJsonObject infoJson;
    infoJson.insert("azs_nodes", jsonAzsNodes);
    infoJson.insert("main_info", mainInfo);

    QString infoText = QString::fromUtf8(QJsonDocument(infoJson).toJson());
    return infoText;
}
