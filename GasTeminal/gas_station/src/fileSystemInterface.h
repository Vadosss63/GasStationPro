#pragma once

#include <QIODevice>
#include <memory>

class FileSystemInterface
{
public:
    virtual ~FileSystemInterface() = default;

    virtual std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode) const             = 0;
    virtual bool                       createDir(const QString& dirPath) const                                   = 0;
    virtual int                        getNumberOfFilesInDir(const QString& dirPath) const                       = 0;
    virtual void                       removeOlderFilesInDir(const QString& dirPath, qint64 maxFileNumber) const = 0;
};
