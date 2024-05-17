#pragma once
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
#include <QObject>
#include <QTimer>

#include "webservercontroller.h"

namespace loguploader
{
class LogUploader : public QObject
{
    Q_OBJECT
public:
    LogUploader(Configure config, QObject* parent = nullptr);

    void run();
    void stop();

private slots:

    void pollServer();

private:
    bool handleServerCommandRequest();
    bool sendLogsToServer();
    void resetServerCommand();

    WebServerController   webServerController;
    QTimer                timer{};
    constexpr static auto folderPath          = "azs_logs";
    constexpr static auto archivePathTemplate = "%1_azsLogs.tar.gz";
    constexpr static auto journalFileTemplate = "%1/%2_journal.log";
};

}
