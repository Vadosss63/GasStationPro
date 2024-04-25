/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "appupdater.h"

#include <QObject>
#include <QProcess>

#include "executor.h"
#include "filesystemutilities.h"
#include "logcommand.h"
#include "logging.h"
#include "utilities.h"
#include "workdirectory.h"

namespace loguploader
{
AppUpdater::AppUpdater(Configure config, QObject* parent) : QObject(parent), webServerController(std::move(config))
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(pollServer()));
}

void AppUpdater::run()
{
    constexpr std::chrono::milliseconds timerInterval{10000};
    timer.start(timerInterval);
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

    const QString& fileUrl = updateCommand->url;
    if (fileUrl.isEmpty())
    {
        return;
    }

    if (!handleUpdateRequest(fileUrl))
    {
        return;
    }

    if (!webServerController.resetServerCmd())
    {
        LOG_ERROR("Error to reset server cmd");
        return;
    }
}

bool AppUpdater::handleUpdateRequest(const QString& fileUrl)
{
    std::unique_ptr<WorkDirectory> workDir = WorkDirectory::create();
    if (!workDir)
    {
        return false;
    }

    const QString          updateDir     = workDir->getWorkDirectory();
    std::optional<QString> savedFilePath = downloadFile(fileUrl, updateDir);
    if (!savedFilePath)
    {
        return false;
    }

    if (!unpackArchive(*savedFilePath, updateDir))
    {
        LOG_ERROR("Error to unpack archive: " + *savedFilePath);
        return false;
    }

    if (!updateApp(updateDir))
    {
        LOG_ERROR("Error to update the app");
        return false;
    }

    return true;
}

std::optional<QString> AppUpdater::downloadFile(const QString& url, const QString& updateDir)
{
    auto data = webServerController.downloadFile(url);
    if (!data)
    {
        LOG_ERROR("Error to download file: " + url);
        return std::nullopt;
    }

    const QUrl    fileUrl{url};
    const QString savedFilePath{updateDir + '/' + fileUrl.fileName()};

    auto file = openFile(savedFilePath, QIODevice::WriteOnly);
    if (!file)
    {
        LOG_ERROR("Error to open file: " + savedFilePath);
        return std::nullopt;
    }

    file->write(data.value());
    file->close();
    LOG_INFO("File is saved: " + savedFilePath);

    return {savedFilePath};
}

bool AppUpdater::writeUpdateResult(const std::string& result)
{
    if (!createDirWithFullPermission(logFolder))
    {
        LOG_WARNING("Failed to create directory for log files");
        return false;
    }

    const auto logFilePath   = QString(logFileTemplate).arg(logFolder).arg(getCurrentTimestamp());
    auto       logFileStream = openFileWithFullPermissions(logFilePath, QIODevice::WriteOnly);

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

bool AppUpdater::updateApp(const QString& updateDir)
{
    const QString     processToExecute{"/bin/bash"};
    const QString     pathToUpdateScript = QString(pathToScript).arg(updateDir);
    const QStringList arguments{"-c", pathToUpdateScript};
    using namespace std::chrono_literals;
    constexpr auto timeout{10min};

    const auto [exitCode, output] = executeProcessWithArgs(processToExecute, arguments, timeout);

    if (!writeUpdateResult(output.toStdString()))
    {
        LOG_WARNING("Failed to write update result");
    }

    if (exitCode)
    {
        LOG_WARNING("Update was ended with an error: " + output);
        return false;
    }

    LOG_INFO("Update was successful");
    return true;
}
}
