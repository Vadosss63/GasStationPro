#include "settings.h"

#include <QErrorMessage>
#include <QFile>
#include <QSettings>
#include <QTextCodec>
#include <QTextStream>

Settings::Settings() {}

Settings& Settings::instance()
{
    static Settings settings;
    return settings;
}

Settings::SettingsPrametrs& Settings::getSettingsPrametrs()
{
    return settingsParameters;
}

QStringList Settings::readLogFile()
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

void Settings::addTextToLogFile(const QString& text)
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
    settingsParameters.sum = 0;
}
