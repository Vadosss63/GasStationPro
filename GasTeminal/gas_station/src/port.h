#pragma once

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
    Port(QObject* parent = nullptr);
    ~Port() override;

    void writeToPort(const QString& data);
    void writeToPort(const QByteArray& data);

    QByteArray getData();

signals:
    void error_(QString err);
    void readyData();

public slots:

    void connectPort();

    void disconnectPort();

    void writeSettingsPort(const QString& name, int baudRate);

    void readData();

private:
    void        checkErrors();
    QByteArray  data;
    Settings    settingsPort;
    QSerialPort serialPort;
};