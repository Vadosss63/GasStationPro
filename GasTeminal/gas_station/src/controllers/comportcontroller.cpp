#include "comportcontroller.h"

#include "logging.h"

ComPortController::ComPortController(const QString& comPort, int baudRate, QObject* parent) : QObject(parent)
{
    connect(&port, SIGNAL(readyData()), this, SLOT(readDataFromPort()));
    /// grep -i 'tty' /var/log/dmesg
    port.setSettingsPort(comPort, baudRate);
    port.connectPort();
}

void ComPortController::sendToPort(const QString& data)
{
    port.writeToPort(data);
}

void ComPortController::sendToPort(const QByteArray& data)
{
    port.writeToPort(data);
    LOG_INFO(convertToString(data));
}

void ComPortController::sendToPort(const std::string& data)
{
    sendToPort(QString::fromStdString(data));
}

void ComPortController::sendResponse()
{
    sendToPort(getResponseData().getQByteArray());
    getResponseData().state = ResponseData::defaultVal;
}

void ComPortController::readDataFromPort()
{
    QByteArray data = port.getData();
    LOG_INFO(convertToString(data));

    ReceivedData* tmp = ReceivedData::getReceivedData(data);
    if (!tmp)
    {
        LOG_ERROR("ReceivedData is invalid");
        return;
    }
    getReceivedData() = *tmp;
    emit readyData();
}

ReceivedData& ComPortController::getReceivedData()
{
    return receiveData;
}

ResponseData& ComPortController::getResponseData()
{
    return sendData;
}
