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

#include "tazs.h"

#include <QApplication>
#include <QErrorMessage>
#include <QKeyEvent>

#include "appsettings.h"
#include "azsstatisticshandler.h"
#include "logging.h"

Tazs::Tazs(QObject* parent) : QObject{parent}
{
    readSettings();
    readConfig();

    setCountAzsNodes(configure.activeBtn2);

    comPortController        = std::make_unique<ComPortController>(configure.comPort, configure.baudRate);
    mainWindowController     = std::make_unique<MainWindowController>(configure, this);
    serviceMenuController    = std::make_unique<ServiceMenuController>();
    webServerController      = std::make_unique<WebServerController>();
    receiptHistoryController = std::make_unique<ReceiptHistoryController>();

    serviceMenuController->createWindow(configure.showSecondPrice, countAzsNode);
    webServerController->setConfigure(configure);
    webServerController->setAzsBtnHandler(this);

    setAzsNode(currentAzsNodes);
    setupSecondPrice();

    connect(serviceMenuController.get(), SIGNAL(setPrice()), this, SLOT(setupPrice()));
    connect(serviceMenuController.get(), SIGNAL(readCounters()), this, SLOT(getCounters()));
    connect(serviceMenuController.get(), SIGNAL(resetCounters()), this, SLOT(resetCounters()));
    connect(serviceMenuController.get(), SIGNAL(showStatistics()), receiptHistoryController.get(), SLOT(showDialog()));
    connect(comPortController.get(), SIGNAL(readyData()), this, SLOT(updateData()));

    connect(mainWindowController.get(), SIGNAL(startFirstAzsNode()), this, SLOT(startFirstAzsNode()));
    connect(mainWindowController.get(), SIGNAL(startSecondAzsNode()), this, SLOT(startSecondAzsNode()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendToServer()));
    timer->start(10000);
    updateData();
    syncAzs();
}

Tazs::~Tazs()
{
    timer->stop();
}

void Tazs::sendToServer()
{
    QString statistics = getJsonReport(getReport());

    webServerController->sendToServer(statistics);
}

void Tazs::run()
{
    mainWindowController->showMainWindow();
}

void Tazs::readConfig()
{
    constexpr auto filePath = "settings.json";

    std::optional<Configure> conf = readConfigure(filePath);
    if (!conf)
    {
        constexpr auto errorMsg = "The settings.json contains invalid fields!";

        std::unique_ptr<QErrorMessage> errorMessage = std::make_unique<QErrorMessage>();
        LOG_ERROR(errorMsg);
        errorMessage->showMessage(errorMsg);
        return;
    }
    configure = conf.value();
}

void Tazs::readSettings()
{
    currentAzsNodes = readAzsNodeSettings();
}

void Tazs::setCountAzsNodes(bool isVisible)
{
    countAzsNode = isVisible ? 2 : 1;
}

void Tazs::handleAzsBtn(const AzsButton& azsButton)
{
    using State = ResponseData::Command;

    switch (azsButton.button)
    {
        case State::setPriceCash1:
        {
            currentAzsNodes.nodes[firstNodeId].priceCash = azsButton.value;
            break;
        }
        case State::setPriceCash2:
        {
            currentAzsNodes.nodes[secondNodeId].priceCash = azsButton.value;
            break;
        }
        case State::setPriceCashless1:
        {
            currentAzsNodes.nodes[firstNodeId].priceCashless = azsButton.value;
            break;
        }
        case State::setPriceCashless2:
        {
            currentAzsNodes.nodes[secondNodeId].priceCashless = azsButton.value;
            break;
        }
        case State::setLockFuelValue1:
        {
            currentAzsNodes.nodes[firstNodeId].lockFuelValue = azsButton.value;
            break;
        }
        case State::setLockFuelValue2:
        {
            currentAzsNodes.nodes[secondNodeId].lockFuelValue = azsButton.value;
            break;
        }
        case State::setGasType1:
        {
            currentAzsNodes.nodes[firstNodeId].gasType = azsButton.value;
            break;
        }
        case State::setGasType2:
        {
            currentAzsNodes.nodes[secondNodeId].gasType = azsButton.value;
            break;
        }
        case State::blockAzsNode:
        {
            mainWindowController->disableAzs(true);
            break;
        }
        case State::unblockAzsNode:
        {
            mainWindowController->disableAzs(false);
            break;
        }
        case State::setFuelArrival1:
            [[fallthrough]];
        case State::setFuelArrival2:
            [[fallthrough]];
        case State::isPressedServiceBtn1:
            [[fallthrough]];
        case State::isPressedServiceBtn2:
            [[fallthrough]];
        case State::isPressedServiceBtn3:
            [[fallthrough]];
        case State::resetCounters:
        {
            break;
        }
        default:
        {
            LOG_ERROR(QString("Press unknown btn: %1").arg(azsButton.button));
            return;
        }
    }

    std::optional<AzsStatistics> azsStatistics = handleAzsStatistics(azsButton);

    if (azsStatistics)
    {
        if (!webServerController->sendAzsStatistics(azsStatistics.value()))
        {
            LOG_WARNING(
                QString("Failed to send statistics to server: %1").arg(azsStatistics.value().getStatisticsJson()));
        }
    }

    comPortController->setCommand(static_cast<ResponseData::Command>(azsButton.button), azsButton.value);

    if (azsButton.value)
    {
        setAzsNode(currentAzsNodes);
        writeSettings();
    }

    LOG_INFO(QString("Received button: %1, value:%2").arg(azsButton.button, azsButton.value));
}

void Tazs::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_1:
        {
            showServiceMenu();
            LOG_INFO("Pressed: show service mode");
            break;
        }
        case Qt::Key_2:
        {
            resetCounters();
            LOG_INFO("Pressed: reset counters");
            break;
        }
        case Qt::Key_3:
        {
            receiptHistoryController->showDialog();
            LOG_INFO("Pressed: history receipt");
            break;
        }
        case Qt::Key_Escape:
        {
            LOG_INFO("Pressed: exit app");
            qApp->exit(0);
            break;
        }
        default:
            break;
    }
}

void Tazs::writeSettings()
{
    writeAzsNodeSettings(currentAzsNodes);
}

void Tazs::setupSecondPrice()
{
    if (!configure.showSecondPrice)
    {
        currentAzsNodes.nodes[firstNodeId].priceCashless  = 0;
        currentAzsNodes.nodes[secondNodeId].priceCashless = 0;
        setAzsNode(currentAzsNodes);
        writeSettings();

        mainWindowController->setCountOfLitres(currentAzsNodes);
    }
}

void Tazs::syncAzs()
{
    comPortController->setCommand(ResponseData::Command::setGasType1, currentAzsNodes.nodes[firstNodeId].gasType);
    comPortController->setCommand(ResponseData::Command::setGasType2, currentAzsNodes.nodes[secondNodeId].gasType);

    comPortController->setCommand(ResponseData::Command::setPriceCash1, currentAzsNodes.nodes[firstNodeId].priceCash);
    comPortController->setCommand(ResponseData::Command::setPriceCash2, currentAzsNodes.nodes[secondNodeId].priceCash);

    comPortController->setCommand(ResponseData::Command::setPriceCashless1,
                                  currentAzsNodes.nodes[firstNodeId].priceCashless);
    comPortController->setCommand(ResponseData::Command::setPriceCashless2,
                                  currentAzsNodes.nodes[secondNodeId].priceCashless);

    comPortController->setCommand(ResponseData::Command::setLockFuelValue1,
                                  currentAzsNodes.nodes[firstNodeId].lockFuelValue);
    comPortController->setCommand(ResponseData::Command::setLockFuelValue2,
                                  currentAzsNodes.nodes[secondNodeId].lockFuelValue);
}

void Tazs::setAzsNode(const AzsNodeSettings& azsNodes)
{
    currentAzsNodes = azsNodes;
    mainWindowController->setAzsNode(azsNodes, configure.showSecondPrice);
}

void Tazs::updateData()
{
    const ReceivedData& data = comPortController->getReceivedData();
    updateStateOfBtn(data.isClickedBtn);
    mainWindowController->setShowData(data);
    mainWindowController->setCountOfLitres(currentAzsNodes);

    checkResetCounters(data);
}

void Tazs::checkResetCounters(const ReceivedData& data)
{
    if (data.dailyCashSum == 0 && data.dailyCoinsSum == 0 && data.dailyCashlessSum == 0 && data.dailyOnlineSum == 0)
    {
        if (oldRecData.dailyCashSum != 0 || oldRecData.dailyCoinsSum != 0 || oldRecData.dailyCashlessSum != 0 ||
            oldRecData.dailyOnlineSum != 0)
        {
            std::optional<AzsStatistics> azsStatistics = getResetAzsStatistics(oldRecData, countAzsNode);

            if (azsStatistics)
            {
                if (!webServerController->sendAzsStatistics(azsStatistics.value()))
                {
                    LOG_WARNING(QString("Failed to send statistics to server: %1")
                                    .arg(azsStatistics.value().getStatisticsJson()));
                }
            }
        }
    }

    oldRecData = data;
}

AzsReport Tazs::getReport() const
{
    return {countAzsNode,
            APP_VERSION,
            mainWindowController->isBlock(),
            comPortController->getReceivedData(),
            currentAzsNodes};
}

void Tazs::showServiceMenu()
{
    serviceMenuController->showServiceMenu(getReport());
}

void Tazs::closeServiceMenu()
{
    serviceMenuController->closeServiceMenu();
}

void Tazs::clickedFirstHWBtn() const
{
    constexpr int idexBtn = 1;
    saveReceipt(idexBtn);
}

void Tazs::clickedSecondHWBtn() const
{
    constexpr int idexBtn = 2;
    saveReceipt(idexBtn);
}

void Tazs::startFirstAzsNode()
{
    comPortController->setCommand(ResponseData::isPressedFirstBtn);
}

void Tazs::startSecondAzsNode()
{
    comPortController->setCommand(ResponseData::isPressedSecondBtn);
}

void Tazs::setupPrice()
{
    handleAzsBtn(serviceMenuController->getAzsButton());
}

void Tazs::updateStateOfBtn(uint8_t isClickedBtn)
{
    using ClickedBtnState = ReceivedData::ClickedBtnState;
    switch (isClickedBtn)
    {
        case ClickedBtnState::Normal:
        {
            break;
        }
        case ClickedBtnState::ShowServiceMode:
        {
            showServiceMenu();
            LOG_INFO("Pressed: show service mode");
            break;
        }
        case ClickedBtnState::CloseServiceMode:
        {
            closeServiceMenu();
            LOG_INFO("Pressed: close service mode");
            break;
        }
        case ClickedBtnState::Button1Pressed:
        {
            clickedFirstHWBtn();
            LOG_INFO("Pressed: button 1");
            break;
        }
        case ClickedBtnState::Button2Pressed:
        {
            clickedSecondHWBtn();
            LOG_INFO("Pressed: button 2");
            break;
        }
        case ClickedBtnState::Unblock:
        {
            break;
        }
        case ClickedBtnState::Block:
        {
            mainWindowController->disableAzs(true);
            LOG_INFO("Pressed: block azs");
            break;
        }
        default:
        {
            LOG_ERROR(
                QString("Pressed: unknown %1 button").arg((int)comPortController->getReceivedData().isClickedBtn));
            break;
        }
    }
}

bool Tazs::isBalanceValid() const
{
    return AppSettings::instance().getSettings().sum > 0;
}

void Tazs::getCounters()
{
    serviceMenuController->setupReportTable(comPortController->getReceivedData());
}

void Tazs::resetCounters()
{
    handleAzsBtn({.idAzs = 0, .value = 0, .button = ResponseData::resetCounters});
}

void Tazs::saveReceipt(int numOfAzsNode) const
{
    webServerController->sendReceiptFiles();

    if (!isBalanceValid())
    {
        LOG_ERROR(QString("Balance is 0 for %1 azsNode").arg(numOfAzsNode));
        return;
    }

    Receipt receipt = mainWindowController->fillReceipt(numOfAzsNode);

    AppSettings::instance().addTextToLogFile(receipt.getReceipt());

    if (!webServerController->sendReceipt(receipt))
    {
        LOG_WARNING(QString("Receipt:%1 isn't sent. It'll be written to file").arg(receipt.time));
        writeReceiptToFile(receipt);
    }
}
