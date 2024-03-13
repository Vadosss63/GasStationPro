#include "webservercontroller.h"

#include "filesystemutilities.h"
#include "logging.h"

namespace loguploader
{
WebServerController::WebServerController(Configure config, QObject* parent)
    : QObject(parent), configure(std::move(config))
{
}

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

    Answer answer = sendPost(configure.host + resetCmdApi, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
    return answer.isOk;
}

std::optional<Answer> WebServerController::readServerCmd()
{
    QUrlQuery params = getUrlQuery();

    Answer answer = sendPost(configure.host + getGetCmdApi, params);
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

}
