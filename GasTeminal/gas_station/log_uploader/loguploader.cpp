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
#include "loguploader.h"

#include <QObject>
#include <chrono>

#include "executor.h"
#include "filesystemutilities.h"
#include "logcommand.h"
#include "logging.h"
#include "utilities.h"

namespace
{
bool writeOutputToFile(const std::string& result, const QString& filePath)
{
    auto fileStream = openFileWithFullPermissions(filePath, QIODevice::WriteOnly);
    if (!fileStream)
    {
        LOG_WARNING("Fail to open file: " + filePath);
        return false;
    }

    constexpr qint64 writeError{-1};
    if (fileStream->write(result.c_str()) == writeError)
    {
        LOG_WARNING(fileStream->errorString());
        return false;
    }

    return true;
}

bool writeSystemJournalToFile(const QString& filePath)
{
    QString                        processToExecute{"/bin/journalctl"};
    QStringList                    arguments{"-b", "-n 10000"};
    constexpr std::chrono::seconds timeout{10};

    const auto [exitCode, output] = executeProcessWithArgs(std::move(processToExecute), std::move(arguments), timeout);
    if (exitCode)
    {
        LOG_WARNING("Fail to read system journal, eror: " + QString::number(exitCode));
        LOG_WARNING("Output : " + output);
        return false;
    }

    return writeOutputToFile(output.toStdString(), filePath);
}
}

namespace loguploader
{
LogUploader::LogUploader(Configure config, QObject* parent) : QObject(parent), webServerController(std::move(config))
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(pollServer()));
}

void LogUploader::run()
{
    constexpr std::chrono::seconds timerInterval{10};
    timer.start(timerInterval);
}

void LogUploader::stop()
{
    timer.stop();
}

void LogUploader::pollServer()
{
    if (!handleServerCommandRequest())
    {
        return;
    }

    sendLogsToServer();

    resetServerCommand();
}

bool LogUploader::handleServerCommandRequest()
{
    std::optional<Answer> answer = webServerController.readServerCmd();
    if (!answer)
    {
        LOG_ERROR("Error to read serverCmd");
        return false;
    }

    std::optional<LogCommand> logCommand = LogCommand::readCommand(answer->msg);
    if (!logCommand)
    {
        LOG_ERROR(QString("Error to read msg: %1").arg(answer->msg));
        return false;
    }

    return logCommand->download;
}

bool LogUploader::sendLogsToServer()
{
    const auto    archivePath   = QString(archivePathTemplate).arg(getCurrentTimestamp());
    const QString currentDir    = currentPath();
    const QString logFolderPath = currentDir + "/" + folderPath;
    const auto    journalPath   = QString(journalFileTemplate).arg(logFolderPath).arg(getCurrentTimestamp());

    if (!writeSystemJournalToFile(journalPath))
    {
        LOG_WARNING("Failed to write system journal to file");
    }

    if (!archiveFolder(logFolderPath, currentDir + "/" + archivePath))
    {
        LOG_ERROR(QString("Error to create %1").arg(archivePath));
        return false;
    }

    const bool isOk = webServerController.sendLogsToServer(archivePath);
    if (!isOk)
    {
        LOG_ERROR(QString("Error to upload %1").arg(archivePath));
    }

    removeFile(archivePath);
    removeFile(journalPath);

    return isOk;
}

void LogUploader::resetServerCommand()
{
    if (!webServerController.resetServerCmd())
    {
        LOG_ERROR("Error to reset server cmd");
    }
}
}
