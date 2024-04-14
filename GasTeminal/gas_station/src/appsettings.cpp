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

#include "appsettings.h"

#include <QSettings>
#include <QTextCodec>
#include <QTextStream>

#include "filesystemutilities.h"

AppSettings::AppSettings() {}

const QString& AppSettings::getReceiptFolderName() const
{
    return receiptFolderName;
}

AppSettings& AppSettings::instance()
{
    static AppSettings settings;
    return settings;
}

AppSettings::Settings& AppSettings::getSettings()
{
    return settings;
}

QStringList AppSettings::readLogFile()
{
    std::unique_ptr<QIODevice> file = openFile(logsName, QIODevice::ReadOnly);
    if (!file)
    {
        return QStringList();
    }
    QString setting = QString::fromUtf8(file->readAll());
    file->close();
    return setting.split(splitter);
}

void AppSettings::addTextToLogFile(const QString& text)
{
    std::unique_ptr<QIODevice> file = openFile(logsName, QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

    if (!file)
    {
        return;
    }
    QTextStream writeStream(file.get());
    writeStream.setCodec(QTextCodec::codecForName("UTF-8"));
    writeStream << text << splitter;
    file->close();
    settings.sum = 0;
}
