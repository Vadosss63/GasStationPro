#include "appupdater.h"

#include <QObject>

#include "filesystemutilities.h"
#include "logcommand.h"
#include "logging.h"
#include "utilities.h"

namespace loguploader
{
AppUpdater::AppUpdater(QObject* parent) : QObject(parent)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(pollServer()));
    constexpr auto filePath = "settings.json";

    std::optional<Configure> conf = readConfigure(filePath);
    if (!conf)
    {
        constexpr auto errorMsg = "The settings.json contains invalid fields!";
        LOG_ERROR(errorMsg);
        return;
    }
    webServerController.setConfigure(conf.value());

    createDirIfNeeded(updatePath);
}

void AppUpdater::run()
{
    timer.start(10000);
}

void AppUpdater::stop()
{
    timer.stop();
}

void AppUpdater::pollServer()
{
    std::optional<Answer> answer = webServerController.readServerCmd();
    if (!answer)
    {
        LOG_ERROR("Error to read serverCmd");
        return;
    }

    std::optional<UpdateCommand> logCommand = UpdateCommand::readCommand(answer.value().msg);

    if (!logCommand)
    {
        LOG_ERROR(QString("Error to read msg: %1").arg(answer.value().msg));
        return;
    }
    const QString& fileUrl = logCommand.value().url;

    if (fileUrl.isEmpty())
    {
        return;
    }

    if (!AppUpdater::downloadFile(fileUrl))
    {
        return;
    }

    if (!webServerController.resetServerCmd())
    {
        LOG_ERROR("Error to reset server cmd");
        return;
    }
}

bool AppUpdater::downloadFile(const QString& url)
{
    const QUrl    fileUrl(url);
    const QString savePath = updatePath + fileUrl.fileName();

    auto data = webServerController.downloadFile(url);

    if (!data)
    {
        LOG_ERROR("Error to download file: " + url);
        return false;
    }

    auto file = openFile(savePath, QIODevice::WriteOnly);
    if (!file)
    {
        LOG_ERROR("Error to open file: " + savePath);
        return false;
    }

    file->write(data.value());
    file->close();
    LOG_INFO("File is saved: " + savePath);

    storedFileName = savePath;

    return true;
}

bool AppUpdater::updateApp()
{
    return true;
}

bool AppUpdater::unpackArchive()
{
    return true;
}
}
