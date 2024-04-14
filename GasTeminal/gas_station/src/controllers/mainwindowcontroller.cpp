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
#include "mainwindowcontroller.h"

#include <QDateTime>

#include "appsettings.h"
#include "fuelutility.h"
#include "priceconvertor.h"
#include "responsedata.h"
#include "utilities.h"

MainWindowController::MainWindowController(const Configure& configure, IKeyPressEvent* iKeyPressEvent, QObject* parent)
    : QObject(parent)
{
    mainWindow.createWidget(configure.showSecondPrice);
    mainWindow.setSupportPhone(configure.phoneOfSupport);
    mainWindow.setVisibleSecondBtn(configure.activeBtn2, configure.showSecondPrice);
    mainWindow.setVisibleSecondBtn(configure.activeBtn2, configure.showSecondPrice);
    mainWindow.setKeyPressEvent(iKeyPressEvent);

    connect(&mainWindow, SIGNAL(startFirstAzsNode()), this, SIGNAL(startFirstAzsNode()));
    connect(&mainWindow, SIGNAL(startSecondAzsNode()), this, SIGNAL(startSecondAzsNode()));
}

void MainWindowController::setShowData(const ReceivedData& data)
{
    setBalance(data.balanceCash, data.balanceCashless, data.balanceOnline);
    setEnabledStart(data);
}

void MainWindowController::setBalance(double inputBalanceCash, double inputBalanceCashless, double inputBalanceOnline)
{
    balanceCash     = inputBalanceCash;
    balanceCashless = inputBalanceCashless;
    balanceOnline   = inputBalanceOnline;
    double balance  = getBalance();
    mainWindow.setBalance(balance);
    AppSettings::instance().getSettings().sum = balance;
}

void MainWindowController::showMainWindow()
{
    mainWindow.showFullScreen();
}

void MainWindowController::setAzsNode(const AzsNodeSettings& azsNodes, bool showSecondPrice)
{
    for (size_t nodeId = 0; nodeId < azsNodes.nodes.size(); ++nodeId)
    {
        auto    type       = convertIntToGasType(azsNodes.nodes[nodeId].gasType);
        QString gasTypeStr = convertGasTypeToString(type);

        mainWindow.setGasType(gasTypeStr, nodeId);

        double priceCash = PriceConvertor::convertToDouble(azsNodes.nodes[nodeId].priceCash);

        if (showSecondPrice)
        {
            mainWindow.setPricePerLitreLableCash(priceCash, nodeId);

            double priceCashless = PriceConvertor::convertToDouble(azsNodes.nodes[nodeId].priceCashless);
            mainWindow.setPricePerLitreLableCashless(priceCashless, nodeId);
        }
        else
        {
            mainWindow.setPricePerLitreLable(priceCash, nodeId);
        }
    }
}

void MainWindowController::disableAzs(bool state)
{
    mainWindow.disableAzs(state);
}

bool MainWindowController::isBlock()
{
    return mainWindow.isBlock();
}

void MainWindowController::setCountOfLitres(const AzsNodeSettings& currentAzsNodes)
{
    for (int nodeId = 0; nodeId < maxAzsNodeCount; ++nodeId)
    {
        double countFuel = 0.f;

        if (mainWindow.isStartBtnEnabled(nodeId))
        {
            double priceCash     = PriceConvertor::convertToDouble(currentAzsNodes.nodes[nodeId].priceCash);
            double priceCashless = PriceConvertor::convertToDouble(currentAzsNodes.nodes[nodeId].priceCashless);

            double countFuelCash     = calculateFuelQuantity(priceCash, balanceCash);
            double countFuelCashless = calculateFuelQuantity(priceCashless, balanceCashless);
            double countFuelOnline   = calculateFuelQuantity(priceCashless, balanceOnline);
            //Price for online is the same like cashless

            countFuel = countFuelCash + countFuelCashless + countFuelOnline;
        }

        countLitres[nodeId] = countFuel;

        mainWindow.setCountLitres(countFuel, nodeId);
    }
}

void MainWindowController::setEnabledStart(const ReceivedData& showData)
{
    for (int nodeId = 0; nodeId < maxAzsNodeCount; ++nodeId)
    {
        bool isEnabled = showData.getIsActiveBtn(nodeId);
        mainWindow.setEnabledStartBtn(isEnabled, nodeId);
    }
}

Receipt MainWindowController::fillReceipt(int numOfAzsNode) const
{
    const int azsNodeIndex = numOfAzsNode - 1;

    return {.time         = currentSecsSinceEpoch(),
            .date         = currentDateTime(),
            .numOfAzsNode = numOfAzsNode,
            .gasType      = mainWindow.getGasTypeStr(azsNodeIndex),
            .countLitres  = countLitres[azsNodeIndex],
            .cash         = balanceCash,
            .cashless     = balanceCashless,
            .online       = balanceOnline,
            .sum          = getBalance()};
}

double MainWindowController::getBalance() const
{
    return balanceCash + balanceCashless + balanceOnline;
}
