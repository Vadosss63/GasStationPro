#include "workdirectory.h"

#include "filesystemutilities.h"

WorkDirectory::WorkDirectory(const QString& workDirectory) : workDir(workDirectory) {}

const QString& WorkDirectory::getWorkDirectory() const
{
    createDirIfNeeded(workDir);
    return workDir;
}

std::unique_ptr<WorkDirectory> WorkDirectory::create()
{
    const QString directory = createTmpUniqueDir();
    if (directory.isEmpty())
    {
        return {};
    }

    return std::unique_ptr<WorkDirectory>(new (std::nothrow) WorkDirectory(directory));
}

WorkDirectory::~WorkDirectory()
{
    removeDirectory(workDir);
}
