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
    QUrlQuery     params    = getUrlQuery();
    const QString getCmdUrl = configure.host + resetCmdApi;

    Answer answer = sendPost(getCmdUrl, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
    return answer.isOk;
}

std::optional<Answer> WebServerController::readServerCmd() const
{
    QUrlQuery     params    = getUrlQuery();
    const QString getCmdUrl = configure.host + getCmdApi;

    Answer answer = sendPost(getCmdUrl, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return std::nullopt;
    }
    LOG_INFO(answer.msg);
    return answer;
}

std::optional<QByteArray> WebServerController::downloadFile(const QString& url) const
{
    return downloadData(url);
}

}
