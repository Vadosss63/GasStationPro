#include "webservercontroller.h"

#include "filesystemutilities.h"
#include "logging.h"

namespace loguploader
{
WebServerController::WebServerController(QObject* parent) : QObject(parent) {}

QUrlQuery loguploader::WebServerController::getUrlQuery() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);

    return params;
}

bool WebServerController::resetServerCmd() const
{
    QUrlQuery params = getUrlQuery();

    Answer answer = sendPost(configure.host + resetLogCmdApi, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
    return answer.isOk;
}

std::optional<Answer> WebServerController::readServerCmd()
{
    QUrlQuery params = getUrlQuery();

    Answer answer = sendPost(configure.host + getLogCmdApi, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return std::nullopt;
    }
    LOG_INFO(answer.msg);
    return answer;
}

std::optional<QByteArray> WebServerController::downloadFile(const QString& url)
{
    return downloadData(url);
}

void WebServerController::setConfigure(const Configure& newConfigure)
{
    configure = newConfigure;
}

}
