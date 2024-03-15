#include "executor.h"

#include <QProcess>

#include "logging.h"

ExecuteResult executeProcessWithArgs(const QString&                   program,
                                     const QStringList&               arguments,
                                     const std::chrono::milliseconds& timeout)
{
    QProcess process{};
    process.start(program, arguments);
    if (!process.waitForFinished(timeout.count()))
    {
        return {-1, process.errorString()};
    }

    return {process.exitCode(), QString(process.readAllStandardOutput().trimmed())};
}
