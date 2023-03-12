#include "appsettings.h"

#include <QErrorMessage>
#include <QFile>
#include <QSettings>
#include <QTextCodec>
#include <QTextStream>

AppSettings::AppSettings() {}

AppSettings& AppSettings::instance()
{
    static AppSettings settings;
    return settings;
}

AppSettings::Settings& AppSettings::getSettings()
{
    return settings;
}

QStringList AppSettings::readLogFile()
{
#ifdef QT_DEBUG
    QFile file("/home/vadosss63/MyProject/GasStationPro/GasTeminal/gas_station/"
               "settings/logs.log");
#else
    QFile file(logsName);
#endif

    file.open(QFile::ReadOnly);
    if (!file.isOpen())
    {
        return QStringList();
    }
    QString setting = QString::fromUtf8(file.readAll());
    file.close();
    return setting.split("####");
}

void AppSettings::addTextToLogFile(const QString& text)
{
#ifdef QT_DEBUG
    QFile file("/home/vadosss63/MyProject/GasStationPro/GasTeminal/logs.log");
#else
    QFile file(logsName);
#endif

    file.open(QIODevice::Append | QIODevice::Text);
    if (!file.isOpen())
    {
        return;
    }
    QTextStream writeStream(&file);
    writeStream.setCodec(QTextCodec::codecForName("UTF-8"));
    writeStream << "\n" << text << "\n####";
    file.close();
    settings.sum = 0;
}
