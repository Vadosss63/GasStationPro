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
#include <string>

#include "webservercontroller.h"

namespace loguploader
{
class AppUpdater : public QObject
{
    Q_OBJECT
public:
    AppUpdater(Configure config, QObject* parent = nullptr);

    void run();
    void stop();

private slots:

    void pollServer();

private:
    bool                   handleUpdateRequest(const QString& fileUrl);
    std::optional<QString> downloadFile(const QString& url, const QString& updateDir);

    bool updateApp(const QString& updateDir);
    bool writeUpdateResult(const std::string& result);

    WebServerController webServerController;
    QTimer              timer{};

    QString storedFileName{};

    static constexpr auto pathToScript    = "%1/*/install_t_azs.sh";
    static constexpr auto logFolder       = "./azs_logs/update_results";
    static constexpr auto logFileTemplate = "%1/%2_upadate_result.log";

    static constexpr qint64 maxLogFileNumber = 5;
};

}
