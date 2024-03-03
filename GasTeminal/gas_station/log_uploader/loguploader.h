#pragma once

#include <QObject>
#include <QTimer>

#include "webservercontroller.h"

namespace loguploader
{
class LogUploader : public QObject
{
    Q_OBJECT
public:
    LogUploader(QObject* parent = nullptr);

    void run();
    void stop();

private slots:

    void pollServer();

private:
    bool sendLogsToServer();

    WebServerController   webServerController{};
    QTimer                timer;
    constexpr static auto folderPath          = "azs_logs";
    constexpr static auto archivePathTemplate = "%1_azsLogs.tar.gz";
};

}
