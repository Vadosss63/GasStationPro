#pragma once

#include <QObject>
#include <QTimer>
#include <string>

#include "webservercontroller.h"

namespace loguploader
{
class AppUpdater : public QObject
{
    Q_OBJECT
public:
    AppUpdater(Configure config, QObject* parent = nullptr);

    void run();
    void stop();

private slots:

    void pollServer();

private:
    bool downloadFile(const QString& url);

    bool updateApp();

    bool writeUpdateResult(const std::string& result);

    bool unpackArchive();

    WebServerController webServerController;
    QTimer              timer{};

    QString storedFileName{};

    static constexpr auto updatePath      = "update/";
    static constexpr auto srcFolder       = "update/src/";
    static constexpr auto logFolder       = "./azs_logs/update_results";
    static constexpr auto logFileTemplate = "%1/%2_upadate_result.log";

    static constexpr qint64 maxLogFileNumber = 5;
};

}
