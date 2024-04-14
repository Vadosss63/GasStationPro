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
#include <optional>

#include "configure.h"
#include "httprequest.h"

namespace loguploader
{
class WebServerController : public QObject
{
    Q_OBJECT
public:
    WebServerController(Configure config, QObject* parent = nullptr);

    std::optional<Answer> readServerCmd() const;

    std::optional<QByteArray> downloadFile(const QString& url) const;

    bool resetServerCmd() const;

private:
    QUrlQuery getUrlQuery() const;

    Configure configure;

    constexpr static auto resetCmdApi = "/reset_app_update_button";
    constexpr static auto getCmdApi   = "/get_app_update_button";
};

}
