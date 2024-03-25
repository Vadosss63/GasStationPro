#pragma once

#include <QObject>

#include "port.h"
#include "receiveddata.h"
#include "responsedata.h"

class ComPortController : public QObject
{
    Q_OBJECT
public:
    ComPortController(const QString& comPort, int baudRate, QObject* parent = nullptr);

    ReceivedData& getReceivedData();

    void setCommand(ResponseData::Command cmd, uint32_t data = 0);

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
    Port         port{};
};
