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
#include <QIODevice>
#include <QUrlQuery>
#include <memory>
#include <optional>

struct Answer
{
    QString msg  = "";
    bool    isOk = false;
};

Answer sendGet(const QString& urlStr);

Answer sendPost(const QString& urlStr, const QUrlQuery& params);

Answer uploadFile(const QString& fileName, std::unique_ptr<QIODevice> file, const QString& serverUrl);

std::optional<QByteArray> downloadData(const QString& urlStr);
