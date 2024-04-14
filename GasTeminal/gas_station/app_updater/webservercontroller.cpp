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
#include "webservercontroller.h"

#include "filesystemutilities.h"
#include "logging.h"

namespace loguploader
{
WebServerController::WebServerController(Configure config, QObject* parent)
    : QObject(parent), configure(std::move(config))
{
}

QUrlQuery loguploader::WebServerController::getUrlQuery() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);

    return params;
}

bool WebServerController::resetServerCmd() const
{
    QUrlQuery     params    = getUrlQuery();
    const QString getCmdUrl = configure.host + resetCmdApi;

    Answer answer = sendPost(getCmdUrl, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
    return answer.isOk;
}

std::optional<Answer> WebServerController::readServerCmd() const
{
    QUrlQuery     params    = getUrlQuery();
    const QString getCmdUrl = configure.host + getCmdApi;

    Answer answer = sendPost(getCmdUrl, params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return std::nullopt;
    }
    LOG_INFO(answer.msg);
    return answer;
}

std::optional<QByteArray> WebServerController::downloadFile(const QString& url) const
{
    return downloadData(url);
}

}
