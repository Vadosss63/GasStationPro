#include "webservercontroller.h"

#include <QFileInfo>

#include "filesystemutilities.h"
#include "logging.h"

namespace loguploader
{
WebServerController::WebServerController(QObject* parent) : QObject(parent) {}

bool WebServerController::resetServerCmd() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);

    Answer answer = sendPost(configure.host + "/reset_log_cmd", params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
    return answer.isOk;
    return true;
}

std::optional<Answer> WebServerController::readServerCmd()
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    Answer answer = sendPost(configure.host + "/get_log_cmd", params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return std::nullopt;
    }
    LOG_INFO(answer.msg);
    return answer;
}

bool WebServerController::sendLogsToServer(const QString& filePath, const QString& serverUrl)
{
    std::unique_ptr<QIODevice> file = openFile(filePath, QIODevice::ReadOnly);

    if (!file)
    {
        return false;
    }
    QString fileName = getFileName(filePath);

    Answer answer = uploadFile(fileName, std::move(file), serverUrl);

    if (!answer.isOk)
    {
        LOG_ERROR("Error upload file to server: " + answer.msg);
    }
    return answer.isOk;
}

void WebServerController::setConfigure(const Configure& newConfigure)
{
    configure = newConfigure;
}

}
