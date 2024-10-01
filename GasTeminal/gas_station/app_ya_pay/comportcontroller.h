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

#include "dataprotocolcomport.h"
#include "logging.h"
#include "port.h"

class ComPortController : public QObject
{
    Q_OBJECT
public:
    ComPortController(const QString& comPort, int baudRate, QObject* parent = nullptr);

    uint8_t getRequestType() const;

    template <typename DataReqType>
    DataReqType getDataReq()
    {
        uint8_t* ptr = reinterpret_cast<uint8_t*>(receiveData.data());
        if (!validateHead(*ptr))
        {
            LOG_ERROR("Invalid head)");
            return {};
        }

        if (!validateChecksum(ptr, receiveData.size()))
        {
            LOG_ERROR("Invalid checksum)");
            return {};
        }

        Package<DataReqType> req = reinterpret_cast<Package<DataReqType>&>(*ptr);
        return req.dataType;
    }

    template <typename DataReqType>
    void sendToPort(const DataReqType& data)
    {
        Package<DataReqType> package;
        package.dataType = data;
        package.addChecksum();
        sendToPort(package.getData());
    }

    const QByteArray& getReceivedData();

    void sendToPort(const QByteArray& data);

    void setTestData(const QByteArray& data)
    {
        ///TODO: to be removed
        receiveData = data;
    }

private slots:
    void readDataFromPort();

signals:
    void readyData();

private:
    void sendToPort(const QString& data);
    void sendToPort(const std::string& data);

    QByteArray receiveData{};
    Port       port{};

    static constexpr int shiftTypeReq = 1;
    static constexpr int shiftData    = 2;
};
