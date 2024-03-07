#pragma once

#include <QIODevice>
#include <QUrlQuery>
#include <memory>
#include <optional>

struct Answer
{
    QString msg  = "";
    bool    isOk = false;
};

Answer sendGet(const QString& urlStr);

Answer sendPost(const QString& urlStr, const QUrlQuery& params);

Answer uploadFile(const QString& fileName, std::unique_ptr<QIODevice> file, const QString& serverUrl);

std::optional<QByteArray> downloadData(const QString& urlStr);
