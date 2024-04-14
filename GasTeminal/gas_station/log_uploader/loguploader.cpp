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
