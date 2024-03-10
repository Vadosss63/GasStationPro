#pragma once

#include <QObject>
#include <optional>

#include "configure.h"
#include "httprequest.h"

namespace loguploader
{
class WebServerController : public QObject
{
    Q_OBJECT
public:
    WebServerController(Configure newConfigure, QObject* parent = nullptr);

    std::optional<Answer> readServerCmd();

    bool sendLogsToServer(const QString& filePath);

    bool resetServerCmd() const;

private:
    QUrlQuery getUrlQuery() const;

    Configure configure;

    constexpr static auto resetLogCmdApi = "/reset_log_cmd";
    constexpr static auto getLogCmdApi   = "/get_log_cmd";
    constexpr static auto uploadLogApi   = "/upload_log";
};

}
