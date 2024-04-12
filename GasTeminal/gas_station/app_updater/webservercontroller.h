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
    WebServerController(Configure config, QObject* parent = nullptr);

    std::optional<Answer> readServerCmd() const;

    std::optional<QByteArray> downloadFile(const QString& url) const;

    bool resetServerCmd() const;

private:
    QUrlQuery getUrlQuery() const;

    Configure configure;

    constexpr static auto resetCmdApi = "/reset_app_update_button";
    constexpr static auto getCmdApi   = "/get_app_update_button";
};

}
