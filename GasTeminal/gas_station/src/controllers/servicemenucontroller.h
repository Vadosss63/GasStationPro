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
#include <QHash>
#include <memory>

#include "azsbutton.h"
#include "azsnodesettings.h"
#include "inputwidget.h"
#include "receiveddata.h"
#include "report.h"
#include "servicemenuwindow.h"

class ServiceMenuController : public QObject
{
    Q_OBJECT
public:
    ServiceMenuController(QObject* parent = nullptr);

    void createWindow(int showSecondPrice, uint8_t countAzsNode);

    void setupReportTable(const ReceivedData& info);

    AzsButton getAzsButton() const;

    void showServiceMenu(const AzsReport& azsReport);
    void closeServiceMenu();

    void setCurrentButtonWidget(QWidget* newCurrentButtonWidget);

public slots:

    void pressedButton();

signals:
    void setPrice();
    void readCounters();
    void resetCounters();
    void showStatistics();

private:
    void setupAzsNodeSettingsGetter();
    void setAzsNodes(const AzsNodeSettings& azsNodes);
    void setButtonValue();

    uint8_t countAzsNode{0};
    int     showSecondPrice{false};

    QHash<int, std::function<int()>>   azsNodeSettingsGetter{};
    std::unique_ptr<ServiceMenuWindow> serviceMenuWindow{nullptr};
    AzsNodeSettings                    azsNodeSettings{};
    AzsButton                          azsButton;
    InputWidget*                       currentButtonWidget{nullptr};
};
