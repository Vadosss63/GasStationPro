#include "appsettings.h"

#include <QSettings>
#include <QTextCodec>
#include <QTextStream>

#include "filesystemutilities.h"

AppSettings::AppSettings() {}

const QString& AppSettings::getReceiptFolderName() const
{
    return receiptFolderName;
}

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
    std::unique_ptr<QIODevice> file = openFile(logsName, QIODevice::ReadOnly);
    if (!file)
    {
        return QStringList();
    }
    QString setting = QString::fromUtf8(file->readAll());
    file->close();
    return setting.split(splitter);
}

void AppSettings::addTextToLogFile(const QString& text)
{
    std::unique_ptr<QIODevice> file = openFile(logsName, QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

    if (!file)
    {
        return;
    }
    QTextStream writeStream(file.get());
    writeStream.setCodec(QTextCodec::codecForName("UTF-8"));
    writeStream << text << splitter;
    file->close();
    settings.sum = 0;
}
