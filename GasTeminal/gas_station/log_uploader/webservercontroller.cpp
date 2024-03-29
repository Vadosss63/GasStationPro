#include "webservercontroller.h"

#include "filesystemutilities.h"
#include "logging.h"

namespace loguploader
{
WebServerController::WebServerController(Configure newConfigure, QObject* parent)
    : QObject(parent), configure(std::move(newConfigure))
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

bool WebServerController::sendLogsToServer(const QString& filePath)
{
    std::unique_ptr<QIODevice> file = openFile(filePath, QIODevice::ReadOnly);

    if (!file)
    {
        LOG_ERROR(QString("Failed to open file: %1").arg(filePath));
        return false;
    }
    const auto serverUrl =
        QString("%1%2?id=%3&token=%4").arg(configure.host, uploadLogApi, configure.id, configure.token);

    const QString fileName = getFileName(filePath);
    const Answer  answer   = uploadFile(fileName, std::move(file), serverUrl);

    if (!answer.isOk)
    {
        LOG_ERROR("Error upload file to server: " + answer.msg);
    }
    return answer.isOk;
}
}
