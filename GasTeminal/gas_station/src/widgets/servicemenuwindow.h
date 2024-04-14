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
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>

#include "azsbuttonwidget.h"

class ServiceMenuWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ServiceMenuWindow(QWidget* parent = nullptr);

    void createNodes(AzsButtonWidget* azsButtonWidget, size_t countNodes);

    void setCommonCash(uint32_t commonCash);
    void setDailyCash(uint32_t dailyCash);

    void setCommonCashless(uint32_t commonCashless);
    void setDailyCashless(uint32_t dailyCashless);

    void setCommonOnline(uint32_t commonOnline);
    void setDailyOnline(uint32_t dailyOnline);

    void setLiters(double common, double daily, size_t nodeId, size_t countAzsNode);

    void setVersion(const QString& ver);

signals:
    void pressedButton();
    void readCounters();
    void resetCounters();
    void showStatistics();

private:
    void createInfoTable(size_t countNodes);

    void setupButtons();
    void setupConnects();

    QPushButton*  setupBtn{nullptr};
    QPushButton*  countersBtn{nullptr};
    QPushButton*  resetCountersBtn{nullptr};
    QPushButton*  statisticsBtn{nullptr};
    QTableWidget* infoTable{nullptr};
    QLabel*       version{nullptr};
};
