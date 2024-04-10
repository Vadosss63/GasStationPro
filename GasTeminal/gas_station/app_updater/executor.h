#pragma once

#include <QString>
#include <QStringList>
#include <chrono>

struct ExecuteResult
{
    int     exitCode{0};
    QString output{};
};

ExecuteResult executeProcessWithArgs(const QString&            program,
                                     const QStringList&        arguments,
                                     std::chrono::milliseconds timeout);
