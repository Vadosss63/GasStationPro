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
#include <utility>

#include "port.h"
#include "receiveddata.h"
#include "responsedata.h"

class ComPortController : public QObject
{
    Q_OBJECT
public:
    ComPortController(const QString& comPort, int baudRate, QObject* parent = nullptr);

    const ReceivedData& getReceivedData();

    void setCommand(ResponseData::Command cmd, uint32_t data = 0);

private slots:
    void readDataFromPort();

signals:
    void readyData();

private:
    void sendResponse();
    void sendToPort(const QString& data);
    void sendToPort(const QByteArray& data);
    void sendToPort(const std::string& data);
    void getCmd();

    ReceivedData receiveData{};
    ResponseData sendData{};
    Port         port{};

    QList<std::pair<ResponseData::Command, uint32_t>> cmdList{};
};
