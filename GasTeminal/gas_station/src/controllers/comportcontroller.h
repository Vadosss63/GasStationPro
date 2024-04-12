#pragma once

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
