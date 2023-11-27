#pragma once

#include <QIODevice>
#include <memory>

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode);
