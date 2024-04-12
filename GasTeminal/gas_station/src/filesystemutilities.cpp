#include "filesystemutilities.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <cerrno>
#include <cstring>
#include <stdlib.h>

#include "logging.h"

namespace
{
constexpr auto fullPermission = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup |
                                QFile::WriteGroup | QFile::ExeGroup | QFile::ReadOther | QFile::WriteOther |
                                QFile::ExeOther;
}

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> file = std::make_unique<QFile>(path);
    if (!file->open(mode))
    {
        LOG_ERROR("Error open file: " + path);
        return {};
    }

    return file;
}

std::unique_ptr<QIODevice> openFileWithFullPermissions(const QString& path, QIODevice::OpenMode mode)
{
    std::unique_ptr<QFile> file = std::make_unique<QFile>(path);
    if (!file->open(mode))
    {
        LOG_ERROR("Error open file: " + path);
        return {};
    }

    if (!file->setPermissions(fullPermission))
    {
        LOG_ERROR("Failed to set permissions for file: " + path);
    }

    return file;
}

bool createDirWithFullPermission(const QString& dirPath)
{
    QDir directory(dirPath);
    if (!directory.exists())
    {
        if (!QDir().mkpath(dirPath))
        {
            LOG_ERROR("Failed to create directory: " + dirPath);
            return false;
        }
    }

    if (!QFile(dirPath).setPermissions(fullPermission))
    {
        LOG_ERROR("Failed to set permissions for directory: " + dirPath);
        removeDirectory(dirPath);
        return false;
    }

    return true;
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

bool removeDirectory(const QString& dirPath)
{
    QDir directory(dirPath);
    if (!directory.exists())
    {
        LOG_WARNING("Failed to remove directory, does not exist: " + dirPath);
        return false;
    }

    if (!directory.removeRecursively())
    {
        LOG_WARNING("Failed to remove directory: " + dirPath);
        return false;
    }

    return true;
}

bool isDirectoryExist(const QString& dirPath)
{
    return QDir(dirPath).exists();
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
        LOG_WARNING("Directory does not exist:" + dirPath);
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
    const QDir directory{dirPath};
    if (!directory.exists())
    {
        LOG_WARNING("Directory does not exist:" + dirPath);
        return;
    }

    const QFileInfoList fileList      = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
    const QFileInfoList filesToRemove = fileList.mid(maxFileNumber);

    for (const auto& file : filesToRemove)
    {
        QFile::remove(file.absoluteFilePath());
    }
}

bool archiveFolder(const QString& folderPath, const QString& archivePath)
{
    if (archivePath.isEmpty())
    {
        LOG_WARNING("Archive path is empty");
        return false;
    }

    QDir folder(folderPath);
    if (!folder.exists())
    {
        LOG_WARNING("Folder does not exist: " + folderPath);
        return false;
    }

    QStringList arguments;
    arguments << "-czf" << archivePath << "-C" << folderPath << ".";

    QProcess process;
    process.start("tar", arguments);
    if (!process.waitForFinished())
    {
        LOG_WARNING("Failed to archive folder: " + process.errorString());
        return false;
    }

    return true;
}

QString getFileName(const QString& filePath)
{
    return QFileInfo(filePath).fileName();
}

std::unique_ptr<QIODevice> tryToOpenLatestFileInDir(const QString& dirPath, QIODevice::OpenMode mode)
{
    const QDir directory{dirPath};
    if (!directory.exists())
    {
        return {};
    }

    const QFileInfoList fileList = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
    if (fileList.isEmpty())
    {
        return {};
    }

    const QFileInfo lastFile = fileList.first();

    return openFile(lastFile.absoluteFilePath(), mode);
}

QString currentPath()
{
    return QDir::currentPath();
}

bool unpackArchive(const QString& archivePath, const QString& srcFolder)
{
    if (archivePath.isEmpty())
    {
        LOG_WARNING("Archive path is empty");
        return false;
    }

    if (!createDirIfNeeded(srcFolder))
    {
        LOG_WARNING("Can't create folder: " + srcFolder);
        return false;
    }

    QStringList arguments;
    arguments << "-xzf" << archivePath << "-C" << srcFolder;

    QProcess process;
    process.start("tar", arguments);
    if (!process.waitForFinished())
    {
        LOG_WARNING("Failed to archive folder: " + process.errorString());
        return false;
    }

    return true;
}

QString createTmpUniqueDir()
{
    char dirTemplate[] = "/tmp/XXXXXX";

    char* uniqueDir = mkdtemp(dirTemplate);
    if (!uniqueDir)
    {
        LOG_ERROR(QString("Failed to create unique tmp dir, errno:") + strerror(errno));
        return {};
    }

    return {dirTemplate};
}
