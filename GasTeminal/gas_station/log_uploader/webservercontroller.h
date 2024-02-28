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
    WebServerController(QObject* parent = nullptr);

    void                  setConfigure(const Configure& newConfigure);
    std::optional<Answer> readServerCmd();

    bool sendLogsToServer(const QString& filePath, const QString& serverUrl);

    bool resetServerCmd() const;

private:
    Configure configure{};
};

}
