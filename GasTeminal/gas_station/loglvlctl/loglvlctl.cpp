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
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <cstdlib>
#include <iostream>

#include "ipstorage.h"
#include "logger_defs.h"

bool writeLoggerLevel(LogLevel logLevel)
{
    QByteArray              byteArray = QByteArray::number(static_cast<ushort>(logLevel));
    static constexpr qint64 writeError{-1};
    IPStorageWriter         writer{loggerLvlStorage};

    return writer.writeData(byteArray) != writeError;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("loglvlctl");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCommandLineParser parser;
    parser.setApplicationDescription("Description: Control tool for the logger severity level");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption levelOption(
        QStringList() << "l"
                      << "level",
        QCoreApplication::translate("main", "Set the logger severity level: DEBUG, INFO, WARNING, ERROR"),
        QCoreApplication::translate("main", "level"),
        "0");
    parser.addOption(levelOption);
    parser.process(app);

    if (!parser.isSet(levelOption))
    {
        std::cerr << "Error: Severity level has to be specified" << std::endl;
        parser.showHelp(EXIT_FAILURE);
    }

    const QString levelStr = parser.value(levelOption).toUpper();

    std::optional<LogLevel> level = logLevelFromString(levelStr);
    if (!level.has_value())
    {
        std::cerr << "Error: Wrong severity level value was passed: " << levelStr.toStdString() << std::endl;
        parser.showHelp(EXIT_FAILURE);
    }

    if (!writeLoggerLevel(*level))
    {
        std::cerr << "Failed to write logger severity level";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
