#include "loguploader.h"

#include <QObject>

#include "filesystemutilities.h"
#include "logcommand.h"
#include "logging.h"
#include "utilities.h"

namespace loguploader
{
LogUploader::LogUploader(Configure config, QObject* parent) : QObject(parent), webServerController(std::move(config))
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(pollServer()));
}

void LogUploader::run()
{
    constexpr int defaultTimeoutMillSec{10000};
    timer.start(defaultTimeoutMillSec);
}

void LogUploader::stop()
{
    timer.stop();
}

void LogUploader::pollServer()
{
    std::optional<Answer> answer = webServerController.readServerCmd();
    if (!answer)
    {
        LOG_ERROR("Error to read serverCmd");
        return;
    }

    std::optional<LogCommand> logCommand = LogCommand::readCommand(answer.value().msg);

    if (!logCommand)
    {
        LOG_ERROR(QString("Error to read msg: %1").arg(answer.value().msg));
        return;
    }

    if (!logCommand.value().download)
    {
        return;
    }

    if (!LogUploader::sendLogsToServer())
    {
        return;
    }

    if (!webServerController.resetServerCmd())
    {
        LOG_ERROR("Error to reset server cmd");
        return;
    }
}

bool LogUploader::sendLogsToServer()
{
    const auto    archivePath = QString(archivePathTemplate).arg(getCurrentTimestamp());
    const QString currentDir  = currentPath();
    if (!archiveFolder(currentDir + "/" + folderPath, currentDir + "/" + archivePath))
    {
        LOG_ERROR(QString("Error to create %1").arg(archivePath));
        return false;
    }

    const bool ok = webServerController.sendLogsToServer(archivePath);

    if (!ok)
    {
        LOG_ERROR(QString("Error to upload %1").arg(archivePath));
    }

    removeFile(archivePath);

    return ok;
}
}
