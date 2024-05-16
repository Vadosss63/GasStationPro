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
#include <cstdint>
#include <optional>

inline constexpr auto loggerLvlStorage = "log_level_storage";

enum class LogLevel : ushort
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};

inline QString logLevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:
            return "DBG";
        case LogLevel::INFO:
            return "INF";
        case LogLevel::WARNING:
            return "WRN";
        case LogLevel::ERROR:
            return "ERR";
        default:
            return "UNKNOWN";
    }
}

inline std::optional<LogLevel> logLevelFromString(const QString& levelStr)
{
    if (levelStr == "DEBUG")
        return LogLevel::DEBUG;
    else if (levelStr == "INFO")
        return LogLevel::INFO;
    else if (levelStr == "WARNING")
        return LogLevel::WARNING;
    else if (levelStr == "ERROR")
        return LogLevel::ERROR;
    else
        return std::nullopt;
}
