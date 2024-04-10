#pragma once

#include <QString>
#include <memory>
#include <string_view>

class WorkDirectory
{
public:
    ~WorkDirectory();
    static std::unique_ptr<WorkDirectory> create();
    const QString&                        getWorkDirectory() const;

private:
    WorkDirectory(const QString& workDirectory);

    QString workDir;
};
