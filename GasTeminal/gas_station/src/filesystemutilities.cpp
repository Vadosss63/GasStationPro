#include "filesystemutilities.h"

#include <QDir>
#include <QFile>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> file = std::make_unique<QFile>(path);

    file->open(mode);

    return file;
}

bool createDir(const QString& dirPath)
{
    QDir directory{dirPath};

    if (!directory.exists())
    {
        return directory.mkdir(dirPath);
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
