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
        jsonAzsNode.insert("fuelArrival", static_cast<int>(azsNode.nodes[i].fuelArrival));

        jsonAzsNode.insert("fuelVolume", convertFromDouble(rec.azsNodes[i].fuelVolume));
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
