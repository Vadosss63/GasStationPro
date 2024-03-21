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
constexpr auto fuelArrivalTemplate   = "fuelArrival%1";

}

void writeAzsNodeSettings(const AzsNodeSettings& azsNodes)
{
    QSettings settings(appName);
    settings.beginGroup(groupName);
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        settings.setValue(QString(gasTypeTemplate).arg(nodeId), azsNodes.nodes[nodeId].gasType);
        settings.setValue(QString(priceCashTemplate).arg(nodeId), azsNodes.nodes[nodeId].priceCash);
        settings.setValue(QString(priceCashlessTemplate).arg(nodeId), azsNodes.nodes[nodeId].priceCashless);
        settings.setValue(QString(lockFuelValueTemplate).arg(nodeId), azsNodes.nodes[nodeId].lockFuelValue);
        settings.setValue(QString(fuelArrivalTemplate).arg(nodeId), azsNodes.nodes[nodeId].fuelArrival);
    }
    settings.endGroup();
}

AzsNodeSettings readAzsNodeSettings()
{
    AzsNodeSettings azsNodes;

    QSettings settings(appName);
    settings.beginGroup(groupName);

    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        azsNodes.nodes[nodeId].gasType       = settings.value(QString(gasTypeTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].priceCash     = settings.value(QString(priceCashTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].priceCashless = settings.value(QString(priceCashlessTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].lockFuelValue = settings.value(QString(lockFuelValueTemplate).arg(nodeId)).toInt();
        azsNodes.nodes[nodeId].fuelArrival   = settings.value(QString(fuelArrivalTemplate).arg(nodeId)).toInt();
    }

    settings.endGroup();

    return azsNodes;
}
