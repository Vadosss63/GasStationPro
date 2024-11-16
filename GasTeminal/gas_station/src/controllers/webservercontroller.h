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
#include <QObject>

#include "azsbuttonhandler.h"
#include "azsstatistics.h"
#include "configure.h"
#include "receipt.h"

class WebServerController : public QObject
{
    Q_OBJECT
public:
    WebServerController(QObject* parent = nullptr);

    void setConfigure(const Configure& newConfigure);

    void sendToServer(const QString& statistics);
    void sendReceiptFiles() const;
    bool sendReceipt(const Receipt& receipt) const;

    bool sendAzsStatistics(AzsStatistics& statistics) const;

    void setBtnFromServer(const AzsButton& azsButton);

    void setAzsBtnHandler(AzsBtnHandler* newAzsBtnHandler);

private:
    void sendReport(const QString& statistics);

    AzsButton getServerBtn() const;

    bool resetServerBtn() const;

    bool sendReceiptFromFile(const QString& fileReceipt) const;

    Configure configure{};
    uint8_t   countAzsNode{2};

    AzsBtnHandler* azsBtnHandler{nullptr};
};
