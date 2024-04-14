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
#include <QString>

#include "logger_defs.h"

void printLog(LogLevel logLevel, const QString& msg, const QString& funcName = "");

#define LOG_DEBUG(msg) printLog(LogLevel::DEBUG, msg, __func__)
#define LOG_INFO(msg) printLog(LogLevel::INFO, msg, __func__)
#define LOG_WARNING(msg) printLog(LogLevel::WARNING, msg, __func__)
#define LOG_ERROR(msg) printLog(LogLevel::ERROR, msg, __func__)

void printLogInf(const QString& log);

void printLogInf(const QByteArray& data);

void printLogErr(const QString& log);

QString convertToString(const QByteArray& data);
