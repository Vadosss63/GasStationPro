#pragma once

#include <QIODevice>
#include <memory>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode);

bool createDir(const QString& dirPath);

void removeFile(const QString& filePath);

QStringList getDirectoryFileList(const QString& dirPath);
