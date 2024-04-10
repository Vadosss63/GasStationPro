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
    bool                   handleUpdateRequest(const QString& fileUrl);
    std::optional<QString> downloadFile(const QString& url, const QString& updateDir);

    bool updateApp(const QString& updateDir);
    bool writeUpdateResult(const std::string& result);

    WebServerController webServerController;
    QTimer              timer{};

    QString storedFileName{};

    static constexpr auto pathToScript    = "%1/*/install_t_azs.sh";
    static constexpr auto logFolder       = "./azs_logs/update_results";
    static constexpr auto logFileTemplate = "%1/%2_upadate_result.log";

    static constexpr qint64 maxLogFileNumber = 5;
};

}
