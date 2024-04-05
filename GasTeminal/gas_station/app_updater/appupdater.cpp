#include "appupdater.h"

#include <QObject>
#include <QProcess>

#include "executor.h"
#include "filesystemutilities.h"
#include "logcommand.h"
#include "logging.h"
#include "utilities.h"

namespace loguploader
{
AppUpdater::AppUpdater(Configure config, QObject* parent) : QObject(parent), webServerController(std::move(config))
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(pollServer()));
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

    std::optional<UpdateCommand> updateCommand = UpdateCommand::readCommand(answer.value().msg);

    if (!updateCommand)
    {
        LOG_ERROR(QString("Error to read msg: %1").arg(answer.value().msg));
        return;
    }
    const QString& fileUrl = updateCommand.value().url;

    if (fileUrl.isEmpty())
    {
        return;
    }

    if (!downloadFile(fileUrl))
    {
        return;
    }

    if (!unpackArchive())
    {
        LOG_ERROR("Error to unpack archive: " + storedFileName);
        return;
    }

    const bool isUpdated = updateApp();
    removeDirectory(updatePath);
    if (!isUpdated)
    {
        LOG_ERROR("Error to update app");
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

bool AppUpdater::writeUpdateResult(const std::string& result)
{
    if (!createDirIfNeeded(logFolder))
    {
        LOG_WARNING("Failed to create directory for log files");
        return false;
    }

    const auto logFilePath   = QString(logFileTemplate).arg(logFolder).arg(getCurrentTimestamp());
    auto       logFileStream = openFile(logFilePath, QIODevice::WriteOnly);

    if (!logFileStream)
    {
        LOG_WARNING("Fail to open log file: " + logFilePath);
        return false;
    }

    constexpr qint64 writeError{-1};
    if (logFileStream->write(result.c_str()) == writeError)
    {
        LOG_WARNING(logFileStream->errorString());
        return false;
    }

    if (const int numberOfFiles = getNumberOfFilesInDir(logFolder); numberOfFiles >= maxLogFileNumber)
    {
        removeOlderFilesInDir(logFolder, maxLogFileNumber);
    }

    return true;
}

bool AppUpdater::updateApp()
{
    const QString     processToExecute{"/bin/bash"};
    const QString     pathToUpdateScript = QString("%1/*/install_t_azs.sh").arg(srcFolder);
    const QStringList arguments{"-c", pathToUpdateScript};
    using namespace std::chrono_literals;
    const std::chrono::milliseconds timeout{120s};

    const auto [exitCode, output] = executeProcessWithArgs(processToExecute, arguments, timeout);

    if (!writeUpdateResult(output.toStdString()))
    {
        LOG_WARNING("Failed to write update result");
    }

    if (exitCode)
    {
        LOG_WARNING("Error to update: " + output);
        return false;
    }

    LOG_INFO("Update result: " + output);
    return true;
}

bool AppUpdater::unpackArchive()
{
    return ::unpackArchive(storedFileName, srcFolder);
}
}
