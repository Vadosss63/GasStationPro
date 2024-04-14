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
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class Port : public QObject
{
    Q_OBJECT

    struct Settings
    {
        QString name;
        int32_t baudRate;
    };

public:
    explicit Port(QObject* parent = nullptr);
    ~Port() override;

    void writeToPort(const QString& data);
    void writeToPort(const QByteArray& data);

    QByteArray getData();

signals:
    void readyData();

public slots:

    void connectPort();

    void disconnectPort();

    void setSettingsPort(const QString& name, int baudRate);

    void readData();

private:
    void        checkErrors();
    QByteArray  data;
    Settings    settingsPort;
    QSerialPort serialPort;
};
