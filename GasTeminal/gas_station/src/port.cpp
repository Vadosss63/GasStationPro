#include "port.h"

#include "logging.h"

Port::Port(QObject* parent) : QObject(parent)
{
    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
}

Port::~Port()
{
    disconnectPort();
}

void Port::writeToPort(const QByteArray& data)
{
    if (!serialPort.isOpen())
    {
        checkErrors();
        return;
    }
    if (!serialPort.write(data))
    {
        checkErrors();
        return;
    }
    serialPort.waitForBytesWritten(10);
    serialPort.flush();
}

void Port::writeToPort(const QString& data)
{
    if (!serialPort.isOpen())
    {
        checkErrors();
        return;
    }
    if (!serialPort.write(data.toStdString().c_str()))
    {
        checkErrors();
        return;
    }
    serialPort.waitForBytesWritten(10);
    serialPort.flush();
}

QByteArray Port::getData()
{
    QByteArray d(qMove(data));
    return d;
}

void Port::setSettingsPort(const QString& name, int baudRate)
{
    settingsPort.name     = name;
    settingsPort.baudRate = static_cast<QSerialPort::BaudRate>(baudRate);
}

void Port::readData()
{
    while (serialPort.bytesAvailable() || serialPort.waitForReadyRead(100))
    {
        data.append(serialPort.readAll());
    }
    emit readyData();
}

void Port::checkErrors()
{
    QString errorMsg = "Error port: " + serialPort.errorString();
    printLogErr(errorMsg);
}

void Port::connectPort()
{
    serialPort.setPortName(settingsPort.name);
    serialPort.setBaudRate(settingsPort.baudRate);

    if (!serialPort.open(QIODevice::ReadWrite))
    {
        printLogErr("No connect to port");
        return;
    }
    printLogInf((settingsPort.name + " >> Open!\r"));
}

void Port::disconnectPort()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
        printLogInf(settingsPort.name.toLocal8Bit() + " >> Close!\r");
    }
}
