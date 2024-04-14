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

#include "logging.h"

#include <iostream>
#include <string>

#include "logger.h"

void printLog(LogLevel logLevel, const QString& msg, const QString& funcName)
{
    constexpr const char* logDirectory = "./azs_logs/gas_station/";
    constexpr const char* fileNamePrefix{"azs_log_file"};
    constexpr qint64      maxLogFileSize{202400};
    constexpr qint64      maxLogFiles{4};
    static Logger         logger{logDirectory, fileNamePrefix, maxLogFileSize, maxLogFiles, LogLevel::DEBUG};
    logger.writeLog(logLevel, msg, funcName);
}

void printLogInf(const QString& log)
{
    std::cout << log.toStdString() << std::endl;
}

void printLogInf(const QByteArray& data)
{
    printLogInf(convertToString(data));
}

void printLogErr(const QString& log)
{
    std::cerr << log.toStdString() << std::endl;
}

QString convertToString(const QByteArray& data)
{
    QString res{};
    for (const auto byte : data)
    {
        res.append("0x" + QString::number(static_cast<uint8_t>(byte), 16) + " ");
    }
    return res;
}
