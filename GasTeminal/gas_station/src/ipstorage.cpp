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

#include "ipstorage.h"

#include <iostream>

IPStorageReader::IPStorageReader(QString name, QObject* parent)
    : QObject(parent), storageName(std::move(name)), server(std::make_unique<QLocalServer>()), socket(nullptr)
{
    QLocalServer::removeServer(storageName);
    if (!server->listen(storageName))
    {
        std::cerr << "Failed to listen local server: " << storageName.toStdString() << std::endl;
    }
    connect(server.get(), SIGNAL(newConnection()), this, SLOT(handleConnection()));
}

void IPStorageReader::handleConnection()
{
    socket.reset(server->nextPendingConnection());
    connect(socket.get(), SIGNAL(readyRead()), this, SLOT(handleData()));
}

void IPStorageReader::handleData()
{
    emit receivedData(socket->readAll());
}

IPStorageWriter::IPStorageWriter(QString name, QObject* parent)
    : QObject(parent), storageName(std::move(name)), socket(std::make_unique<QLocalSocket>())
{
}

qint64 IPStorageWriter::writeData(const QByteArray& data)
{
    if (socket->connectToServer(storageName); !socket->waitForConnected())
    {
        std::cerr << "Failed to connect to storage" << std::endl;
        return -1;
    }

    const qint64 writedBytes = socket->write(data);
    socket->flush();
    socket->abort();

    return writedBytes;
}
