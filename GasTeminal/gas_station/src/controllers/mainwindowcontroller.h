#pragma once
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
#include "configure.h"
#include "constants.h"
#include "mainwindow.h"
#include "receipt.h"
#include "receiveddata.h"

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    MainWindowController(const Configure& configure, IKeyPressEvent* iKeyPressEvent, QObject* parent = nullptr);
    ~MainWindowController() override = default;

    void showMainWindow();

    void setAzsNode(const AzsNodeSettings& azsNodes, bool showSecondPrice);

    void disableAzs(bool state);

    bool isBlock();

    void setCountOfLitres(const AzsNodeSettings& currentAzsNodes);

    void setShowData(const ReceivedData& data);

    Receipt fillReceipt(int numOfAzsNode) const;

signals:
    void startFirstAzsNode();
    void startSecondAzsNode();

private:
    double getBalance() const;

    void setEnabledStart(const ReceivedData& showData);

    void setBalance(double inputBalanceCash, double inputBalanceCashless, double inputBalanceOnline);

    MainWindow mainWindow;

    double                              balanceCashless{0};
    double                              balanceCash{0};
    double                              balanceOnline{0};
    std::array<double, maxAzsNodeCount> countLitres{};
};
