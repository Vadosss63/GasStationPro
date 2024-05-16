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

#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QString>
#include <QtNetwork>
#include <memory>

class IPStorageReader : public QObject
{
    Q_OBJECT
public:
    explicit IPStorageReader(QString name, QObject* parent = nullptr);

signals:
    void receivedData(const QByteArray& data);
private slots:
    void handleConnection();
    void handleData();

private:
    QString                       storageName;
    std::unique_ptr<QLocalServer> server;
    std::unique_ptr<QLocalSocket> socket;
};

class IPStorageWriter : public QObject
{
    Q_OBJECT
public:
    explicit IPStorageWriter(QString name, QObject* parent = nullptr);

    qint64 writeData(const QByteArray& data);

private:
    QString                       storageName;
    std::unique_ptr<QLocalSocket> socket;
};
