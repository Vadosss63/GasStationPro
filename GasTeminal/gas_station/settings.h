#pragma once
#include <QRect>
#include <QString>

class Settings {
public:
  struct SettingsPrametrs {
    int sum = 0;
  };

  static Settings &instance();

  SettingsPrametrs &getSettingsPrametrs();

  QStringList readLogFile();
  void addTextToLogFile(const QString &text);

private:
  Settings();
  SettingsPrametrs settingsParameters;
  const QString logsName = "logs.log";
};
