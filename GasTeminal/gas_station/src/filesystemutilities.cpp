#include "filesystemutilities.h"

#include <QDir>
#include <QFile>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> file = std::make_unique<QFile>(path);

    file->open(mode);

    return file;
}

bool createDirIfNeeded(const QString& dirPath)
{
    QDir directory(dirPath);
    if (!directory.exists())
    {
        return QDir().mkpath(dirPath);
    }

    return true;
}

void removeFile(const QString& filePath)
{
    QFile file{filePath};
    file.remove();
}

QStringList getDirectoryFileList(const QString& dirPath)
{
    QDir directory{dirPath};

    if (!directory.exists())
    {
        return {};
    }

    return directory.entryList(QDir::Files | QDir::NoSymLinks);
}

qint64 getFileSize(const QString& filePath)
{
    QFile file{filePath};
    return file.size();
}

int getNumberOfFilesInDir(const QString& dirPath)
{
    return getDirectoryFileList(dirPath).size();
}

void removeOlderFilesInDir(const QString& dirPath, qint64 maxFileNumber)
{
    QDir directory{dirPath};
    if (!directory.exists())
    {
        return;
    }

    const QFileInfoList fileList      = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
    const QFileInfoList filesToRemove = fileList.mid(maxFileNumber);

    for (const auto& file : filesToRemove)
    {
        QFile::remove(file.absoluteFilePath());
    }
}
