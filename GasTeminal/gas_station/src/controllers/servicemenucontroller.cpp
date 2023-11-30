#include "servicemenucontroller.h"

#include "price.h"

ServiceMenuController::ServiceMenuController(QObject* parent) : QObject(parent) {}

void ServiceMenuController::createWindow(int showSecondPrice, uint8_t countAzsNode)
{
    serviceMenuWindow = std::make_unique<ServiceMenuWindow>();
    //TODO: make refactoring
    if (countAzsNode == 1 && showSecondPrice == false)
    {
        serviceMenuWindow->createOnePriceOneNode();
    }
    else if (countAzsNode == 2 && showSecondPrice == false)
    {
        serviceMenuWindow->createOnePriceTwoNode();
    }
    else if (countAzsNode == 1 && showSecondPrice == true)
    {
        serviceMenuWindow->createTwoPriceOneNode();
    }
    else if (countAzsNode == 2 && showSecondPrice == true)
    {
        serviceMenuWindow->createTwoPriceTwoNode();
    }

    this->showSecondPrice = showSecondPrice;
    this->countAzsNode    = countAzsNode;

    connect(serviceMenuWindow.get(), SIGNAL(setPrice()), this, SLOT(setupPrice()));
    connect(serviceMenuWindow.get(), SIGNAL(readCounters()), this, SIGNAL(readCounters()));
    connect(serviceMenuWindow.get(), SIGNAL(resetCounters()), this, SIGNAL(resetCounters()));
    connect(serviceMenuWindow.get(), SIGNAL(showStatistics()), this, SIGNAL(showStatistics()));
}

void ServiceMenuController::setupReportTable(const ReceivedData& info)
{
    serviceMenuWindow->setCommonCash(info.commonCashSum + info.commonCoinsSum);
    serviceMenuWindow->setDailyCash(info.dailyCashSum + info.dailyCoinsSum);

    serviceMenuWindow->setCommonCashless(info.commonCashlessSum);
    serviceMenuWindow->setDailyCashless(info.dailyCashlessSum);

    serviceMenuWindow->setCommonOnline(info.commonOnlineSum);
    serviceMenuWindow->setDailyOnline(info.dailyOnlineSum);
    for (int nodeId = 0; nodeId < countAzsNode; ++nodeId)
    {
        double common = static_cast<double>(info.azsNodes[nodeId].common) / 100.;
        double daily  = static_cast<double>(info.azsNodes[nodeId].daily) / 100.;
        serviceMenuWindow->setLiters(common, daily, nodeId, countAzsNode);
    }
}

void ServiceMenuController::showServiceMenu(const ReceivedData&                                       info,
                                            const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    setAzsNodes(azsNodes);
    setupReportTable(info);
    serviceMenuWindow->show();
    serviceMenuWindow->setFocus();
}

void ServiceMenuController::setPriceCashToServiceMenu(size_t nodeId, uint16_t price)
{
    Price priceCash(price);
    serviceMenuWindow->setPriceCashRub(priceCash.getRub(), nodeId);
    serviceMenuWindow->setPriceCashKop(priceCash.getKop(), nodeId);
}

void ServiceMenuController::setPriceCashlessToServiceMenu(size_t nodeId, uint16_t price)
{
    if (!showSecondPrice)
    {
        return;
    }
    Price priceCashless(price);
    serviceMenuWindow->setPriceCashlessRub(priceCashless.getRub(), nodeId);
    serviceMenuWindow->setPriceCashlessKop(priceCashless.getKop(), nodeId);
}

void ServiceMenuController::setGasTypeToServiceMenu(size_t nodeId, ResponseData::GasType gasType)
{
    QString gasTypeStr = getGasTypeString(gasType);
    serviceMenuWindow->setGasType(gasTypeStr, nodeId);
}

void ServiceMenuController::setAzsNodes(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    for (int nodeId = 0; nodeId < countAzsNode; ++nodeId)
    {
        setPriceCashToServiceMenu(nodeId, azsNodes[nodeId].priceCash);
        setPriceCashlessToServiceMenu(nodeId, azsNodes[nodeId].priceCashless);
        setGasTypeToServiceMenu(nodeId, azsNodes[nodeId].gasType);
    }
}

void ServiceMenuController::setupPrice()
{
    for (int nodeId = 0; nodeId < countAzsNode; ++nodeId)
    {
        setPriceCashToAzsNodes(nodeId);
        setPriceCashlessToAzsNodes(nodeId);
        setGasTypeToAzsNode(nodeId);
    }
    emit setPrice();
    closeServiceMenu();
}

std::array<ResponseData::AzsNode, countAzsNodeMax> ServiceMenuController::getAzsNodes() const
{
    return azsNodes;
}

void ServiceMenuController::setPriceCashlessToAzsNodes(size_t nodeId)
{
    if (!showSecondPrice)
    {
        return;
    }
    uint16_t rub = serviceMenuWindow->getPriceCashlessRub(nodeId);
    uint16_t kop = serviceMenuWindow->getPriceCashlessKop(nodeId);
    Price    priceCashless(rub, kop);
    azsNodes[nodeId].priceCashless = priceCashless.getPriceInt();
}

void ServiceMenuController::setPriceCashToAzsNodes(size_t nodeId)
{
    uint16_t rub = serviceMenuWindow->getPriceCashRub(nodeId);
    uint16_t kop = serviceMenuWindow->getPriceCashKop(nodeId);
    Price    priceCash(rub, kop);
    azsNodes[nodeId].priceCash = priceCash.getPriceInt();
}

void ServiceMenuController::setGasTypeToAzsNode(size_t nodeId)
{
    int type = serviceMenuWindow->getGasType(nodeId);

    azsNodes[nodeId].gasType = static_cast<ResponseData::GasType>(type);
}

void ServiceMenuController::closeServiceMenu()
{
    serviceMenuWindow->close();
}
