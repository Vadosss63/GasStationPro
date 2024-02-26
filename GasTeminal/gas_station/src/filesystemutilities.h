#pragma once

#include <QIODevice>
#include <memory>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode);

std::unique_ptr<QIODevice> tryToOpenLatestFileInDir(const QString& dirPath, QIODevice::OpenMode mode);

bool createDirIfNeeded(const QString& dirPath);

bool isDirectoryExist(const QString& dirPath);

void removeFile(const QString& filePath);

QStringList getDirectoryFileList(const QString& dirPath);

qint64 getFileSize(const QString& filePath);

int getNumberOfFilesInDir(const QString& dirPath);

void removeOlderFilesInDir(const QString& dirPath, qint64 maxFileNumber);
