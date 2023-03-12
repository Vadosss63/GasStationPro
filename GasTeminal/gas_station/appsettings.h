#pragma once
#include <QString>

class AppSettings
{
public:
    struct Settings
    {
        int sum = 0;
    };

    static AppSettings& instance();

    Settings& getSettings();

    QStringList readLogFile();
    void        addTextToLogFile(const QString& text);

private:
    AppSettings();
    Settings      settings;
    const QString logsName = "logs.log";
};
