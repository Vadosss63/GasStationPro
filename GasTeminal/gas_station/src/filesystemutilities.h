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
#include <memory>
#include <optional>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode);

std::unique_ptr<QIODevice> openFileWithFullPermissions(const QString& path, QIODevice::OpenMode mode);

QString getFileName(const QString& filePath);

QString currentPath();

std::unique_ptr<QIODevice> tryToOpenLatestFileInDir(const QString& dirPath, QIODevice::OpenMode mode);

bool createDirIfNeeded(const QString& dirPath);

bool createDirWithFullPermission(const QString& dirPath);

bool removeDirectory(const QString& dirPath);

bool isDirectoryExist(const QString& dirPath);

void removeFile(const QString& filePath);

QStringList getDirectoryFileList(const QString& dirPath);

qint64 getFileSize(const QString& filePath);

int getNumberOfFilesInDir(const QString& dirPath);

void removeOlderFilesInDir(const QString& dirPath, qint64 maxFileNumber);

bool archiveFolder(const QString& folderPath, const QString& archivePath);

bool unpackArchive(const QString& archivePath, const QString& srcFolder);

QString createTmpUniqueDir();
