#include "report.h"

QString getJsonReport(size_t countNode, const ReceivedData& rec, const AzsNodeSettings& azsNode)
{
    QJsonObject mainInfo;
    mainInfo.insert("commonCash", static_cast<int>(rec.commonCashSum + rec.commonCoinsSum));
    mainInfo.insert("dailyCash", static_cast<int>(rec.dailyCashSum + rec.dailyCoinsSum));

    mainInfo.insert("commonCashless", static_cast<int>(rec.commonCashlessSum));
    mainInfo.insert("dailyCashless", static_cast<int>(rec.dailyCashlessSum));

    mainInfo.insert("commonOnline", static_cast<int>(rec.commonOnlineSum));
    mainInfo.insert("dailyOnline", static_cast<int>(rec.dailyOnlineSum));

    QJsonArray jsonAzsNodes;
    for (size_t i = 0; i < countNode; ++i)
    {
        QJsonObject jsonAzsNode;
        jsonAzsNode.insert("commonLiters",
                           QString("%1").arg(static_cast<double>(rec.azsNodes[i].common) / 100., 0, 'f', 2));
        jsonAzsNode.insert("dailyLiters",
                           QString("%1").arg(static_cast<double>(rec.azsNodes[i].daily) / 100., 0, 'f', 2));

        auto type = convertIntToGasType(azsNode.nodes[i].gasType);
        jsonAzsNode.insert("typeFuel", convertGasTypeToString(type));
        jsonAzsNode.insert("price", static_cast<int>(azsNode.nodes[i].priceCash));
        jsonAzsNode.insert("priceCashless", static_cast<int>(azsNode.nodes[i].priceCashless));
        jsonAzsNode.insert("fuelVolume", QString("%1").arg(rec.azsNodes[i].fuelVolume, 0, 'f', 2));
        jsonAzsNode.insert("fuelVolumePerc", QString("%1").arg(rec.azsNodes[i].fuelVolumePerc, 0, 'f', 2));
        jsonAzsNode.insert("density", QString("%1").arg(rec.azsNodes[i].density, 0, 'f', 2));
        jsonAzsNode.insert("averageTemperature", QString("%1").arg(rec.azsNodes[i].averageTemperature, 0, 'f', 2));
        jsonAzsNodes.append(jsonAzsNode);
    }

    QJsonObject infoJson;
    infoJson.insert("azs_nodes", jsonAzsNodes);
    infoJson.insert("main_info", mainInfo);

    QString infoText = QString::fromUtf8(QJsonDocument(infoJson).toJson());
    return infoText;
}
