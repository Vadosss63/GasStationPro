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
#include <QPushButton>

#include "constants.h"
#include "ikeypressevent.h"
#include "labelwidget.h"
#include "temporarilyunavailablewidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    struct AzsNodeWidget
    {
        LabelWidget* gasTypeLable{nullptr};
        LabelWidget* pricePerLitreLableCash{nullptr};
        LabelWidget* pricePerLitreLableCashless{nullptr};
        QPushButton* startBtn{nullptr};
        LabelWidget* countLitresLable{nullptr};
    };

public:
    MainWindow();
    ~MainWindow() override;

    QString getCountLitresStr(size_t nodeId) const;

    QString getGasTypeStr(size_t nodeId) const;

    void setGasType(QString gasType, size_t nodeId);

    void setPricePerLitreLableCash(double priceCash, size_t nodeId);

    void setSupportPhone(const QString& phone);

    void createWidget(bool showSecondPrice);

    void disableAzs(bool disable);

    bool isBlock();

    void setPricePerLitreLable(double priceCash, size_t nodeId);
    void setPricePerLitreLableCashless(double price, size_t nodeId);

    void setVisibleSecondBtn(bool isVisible, bool showSecondPrice);

    void setBalance(double balance);

    void setCountLitres(double countFuel, size_t nodeId);
    bool isStartBtnEnabled(size_t nodeId);

    void setKeyPressEvent(IKeyPressEvent* newIKeyPressEvent);

    void setEnabledStartBtn(bool isEnabled, size_t nodeId);

signals:
    void startFirstAzsNode();
    void startSecondAzsNode();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    TemporarilyUnavailableWidget* temporarilyUnavailableWidget{nullptr};

    IKeyPressEvent* iKeyPressEvent{nullptr};

    LabelWidget* balanceLable{nullptr};
    LabelWidget* phoneOfSupportLable{nullptr};

    std::array<AzsNodeWidget, maxAzsNodeCount> azsNodeWidgets{};
};
