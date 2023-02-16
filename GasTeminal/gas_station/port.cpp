#include "port.h"

#include <utility>

#include <qdebug.h>

Port::Port(QObject* parent) : QObject(parent)
{
    connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
}

Port::~Port()
{
    if (m_serialPort.isOpen())
    {
        m_serialPort.close();
    }
}

void Port::writeToPort(const QByteArray& data)
{
    if (!m_serialPort.isOpen())
    {
        checkErrors();
        return;
    }
    if (!m_serialPort.write(data))
    {
        checkErrors();
        return;
    }
    m_serialPort.waitForBytesWritten(10); // ждем пока дойдет
    m_serialPort.flush();
}

void Port::writeToPort(const QString& data)
{
    if (!m_serialPort.isOpen())
    {
        checkErrors();
        return;
    }
    if (!m_serialPort.write(data.toStdString().c_str()))
    {
        checkErrors();
        return;
    }
    m_serialPort.waitForBytesWritten(10); // ждем пока дойдет
    m_serialPort.flush();
}

QByteArray Port::getData()
{
    QByteArray data(qMove(m_data));
    return data;
}

void Port::writeSettingsPort(QString name, int baudrate)
{
    qDebug("writeSettingsPort");
    m_settingsPort.name     = std::move(name);
    m_settingsPort.baudRate = static_cast<QSerialPort::BaudRate>(baudrate);
}

void Port::readData()
{
    while (m_serialPort.bytesAvailable() || m_serialPort.waitForReadyRead(100))
    {
        m_data.append(m_serialPort.readAll());
    }
    emit readyData();
}

void Port::checkErrors()
{
    QString errorMsg = "Error potr: " + m_serialPort.errorString();
    error_(errorMsg);
}

void Port::connectPort()
{
    // создали экземпляр для общения по последовательному порту
    // указали имя к какому порту будем подключаться
    m_serialPort.setPortName(m_settingsPort.name);
    // указали скорость
    m_serialPort.setBaudRate(m_settingsPort.baudRate);

    // пробуем подключится
    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        // если подключится не получится, то покажем сообщение с ошибкой
        emit error_("No connect to port");
        return;
    }
    error_((m_settingsPort.name + " >> Open!\r"));
}

void Port::disconnectPort()
{
    if (m_serialPort.isOpen())
    {
        m_serialPort.close();
        error_(m_settingsPort.name.toLocal8Bit() + " >> Close!\r");
    }
}
