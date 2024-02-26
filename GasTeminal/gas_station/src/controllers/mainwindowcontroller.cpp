#include "mainwindowcontroller.h"

#include <QDateTime>
#include <QErrorMessage>
#include <QKeyEvent>
#include <QUrlQuery>

#include "appsettings.h"
#include "filesystemutilities.h"
#include "logger.h"
#include "price.h"

MainWindowController::MainWindowController(const Configure& configure, IKeyPressEvent* iKeyPressEvent, QObject* parent)
    : QObject(parent)
{
    mainWindow.createWidget(configure.showSecondPrice);
    mainWindow.setSupportPhone(configure.phoneOfSupport);
    mainWindow.setVisibleSecondBtn(configure.activeBtn2, configure.showSecondPrice);
    mainWindow.setVisibleSecondBtn(configure.activeBtn2, configure.showSecondPrice);
    mainWindow.setKeyPressEvent(iKeyPressEvent);

    setBalance(0, 0);
    setShowData(ReceivedData{});

    connect(&mainWindow, SIGNAL(startFirstAzsNode()), this, SIGNAL(startFirstAzsNode()));
    connect(&mainWindow, SIGNAL(startFirstAzsNode()), this, SIGNAL(startSecondAzsNode()));
}

MainWindowController::~MainWindowController() {}

void MainWindowController::setShowData(const ReceivedData& data)
{
    setBalance(data.balanceCash, data.balanceCashless);
    setEnabledStart(data);
}

void MainWindowController::setBalance(double inputBalanceCash, double inputBalanceCashless)
{
    balanceCash     = inputBalanceCash;
    balanceCashless = inputBalanceCashless;
    double price    = inputBalanceCash + inputBalanceCashless;
    mainWindow.setBalance(price);
    AppSettings::instance().getSettings().sum = price;
}

void MainWindowController::showMainWindow()
{
    mainWindow.showFullScreen();
}

void MainWindowController::setAzsNode(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes,
                                      bool                                                      showSecondPrice)
{
    for (size_t nodeId = 0; nodeId < azsNodes.size(); ++nodeId)
    {
        QString gasTypeStr = convertGasTypeToString(azsNodes[nodeId].gasType);

        mainWindow.setGasType(gasTypeStr, nodeId);

        double priceCash = Price::convertPriceToDouble(azsNodes[nodeId].priceCash);

        if (showSecondPrice)
        {
            mainWindow.setPricePerLitreLableCash(priceCash, nodeId);

            double priceCashless = Price::convertPriceToDouble(azsNodes[nodeId].priceCashless);
            mainWindow.setPricePerLitreLableCashless(priceCashless, nodeId);
        }
        else
        {
            mainWindow.setPricePerLitreLable(priceCash, nodeId);
        }
    }
}

void MainWindowController::checkNeedToDisableAzs(ResponseData::State state)
{
    if (ResponseData::State::blockAzsNode == state)
    {
        mainWindow.disableAzs(true);
    }
    else if (ResponseData::State::unblockAzsNode == state)
    {
        mainWindow.disableAzs(false);
    }
}

void MainWindowController::setCountOfLitres(const std::array<ResponseData::AzsNode, countAzsNodeMax>& currentAzsNodes)
{
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        double priceCash     = Price::convertPriceToDouble(currentAzsNodes[nodeId].priceCash);
        double priceCashless = Price::convertPriceToDouble(currentAzsNodes[nodeId].priceCashless);

        bool isStartBtnEnabled = mainWindow.isStartBtnEnabled(nodeId);

        double countFuelCash     = (isStartBtnEnabled && priceCash) ? (balanceCash / priceCash) : 0;
        double countFuelCashless = (isStartBtnEnabled && priceCashless) ? (balanceCashless / priceCashless) : 0;

        double countFuel = countFuelCash + countFuelCashless;

        mainWindow.setCountLitres(countFuel, nodeId);
    }
}

void MainWindowController::setEnabledStart(const ReceivedData& showData)
{
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        bool isEnabled = showData.getIsActiveBtn(nodeId);
        mainWindow.setEnabledStartBtn(isEnabled, nodeId);
    }
}

Receipt MainWindowController::fillReceipt(int numOfAzsNode) const
{
    const int azsNodeIndex = numOfAzsNode - 1;

    Receipt receipt{};

    receipt.time         = static_cast<int>(QDateTime::currentSecsSinceEpoch());
    receipt.date         = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    receipt.numOfAzsNode = numOfAzsNode;
    receipt.gasType      = mainWindow.getGasTypeStr(azsNodeIndex);
    receipt.countLitres  = mainWindow.getCountLitresStr(azsNodeIndex);
    receipt.cash         = balanceCash;
    receipt.cashless     = balanceCashless;
    receipt.sum          = mainWindow.getSumStr();
    return receipt;
}
