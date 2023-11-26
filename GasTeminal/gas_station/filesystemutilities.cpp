#include "filesystemutilities.h"

#include <QFile>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> file = std::make_unique<QFile>(path);

    file->open(mode);

    return file;
}
