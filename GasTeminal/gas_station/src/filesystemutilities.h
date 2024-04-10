#pragma once

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
