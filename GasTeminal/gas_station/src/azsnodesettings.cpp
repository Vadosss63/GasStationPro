#include "azsnodesettings.h"

#include <QSettings>

namespace
{
constexpr auto appName   = "Gas Station";
constexpr auto groupName = "parms";

constexpr auto priceCashTemplate     = "currentPriceCash%1";
constexpr auto priceCashlessTemplate = "currentPriceCashless%1";
constexpr auto gasTypesTemplate      = "gasTypes%1";

}

void writeAzsNodeSettings(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    QSettings settings(appName);
    settings.beginGroup(groupName);
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        settings.setValue(QString(priceCashTemplate).arg(nodeId), azsNodes[nodeId].priceCash);
        settings.setValue(QString(priceCashlessTemplate).arg(nodeId), azsNodes[nodeId].priceCashless);
        settings.setValue(QString(gasTypesTemplate).arg(nodeId), azsNodes[nodeId].gasType);
    }
    settings.endGroup();
}

std::array<ResponseData::AzsNode, countAzsNodeMax> readAzsNodeSettings()
{
    std::array<ResponseData::AzsNode, countAzsNodeMax> azsNodes;

    QSettings settings(appName);
    settings.beginGroup(groupName);

    bool ok = false;
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        azsNodes[nodeId].priceCash     = settings.value(QString(priceCashTemplate).arg(nodeId)).toInt();
        azsNodes[nodeId].priceCashless = settings.value(QString(priceCashlessTemplate).arg(nodeId)).toInt();

        int gasTypeInt           = QString(gasTypesTemplate).arg(nodeId).toUInt(&ok);
        azsNodes[nodeId].gasType = static_cast<ResponseData::GasType>(gasTypeInt);
    }

    settings.endGroup();

    return azsNodes;
}
