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

void ComPortController::getCmd()
{
    if (receiveData.command != sendData.command)
    {
        LOG_ERROR(QString("Command %1 isn't sent. Retry sending").arg(sendData.command));
        return;
    }

    if (cmdList.empty())
    {
        sendData.command = ResponseData::Command::defaultVal;
        sendData.data    = 0;
        return;
    }
    const auto& [cmd, data] = cmdList.front();
    sendData.command        = cmd;
    sendData.data           = data;
    cmdList.pop_front();
}

void ComPortController::sendResponse()
{
    getCmd();
    sendToPort(sendData.getQByteArray());
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
    receiveData = *tmp;
    emit readyData();

    sendResponse();
}

const ReceivedData& ComPortController::getReceivedData()
{
    return receiveData;
}

void ComPortController::setCommand(ResponseData::Command cmd, uint32_t data)
{
    cmdList.push_back({cmd, data});
}
