#pragma once

#include <QObject>

#include <port.h>

#include "dataprotocol.h"

class ComPortController : public QObject
{
    Q_OBJECT
public:
    ComPortController(const QString& comPort, int baudRate, QObject* parent = nullptr);
    ResponseData& getResponseData();
    ReceivedData& getReceivedData();

    void sendResponse();

private slots:
    void readDataFromPort();

signals:
    void readyData();

private:
    void sendToPort(const QString& data);
    void sendToPort(const QByteArray& data);
    void sendToPort(const std::string& data);

    ReceivedData receiveData{};
    ResponseData sendData{};
    Port         port;
};
