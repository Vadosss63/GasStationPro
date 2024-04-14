/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "httprequest.h"

#include <QEventLoop>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <functional>

#include "logging.h"

namespace
{
std::pair<QByteArray, bool> sendReq(
    const QString&                                                                          urlStr,
    std::function<std::unique_ptr<QNetworkReply>(QNetworkAccessManager&, QNetworkRequest&)> funcReq)
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop{};
    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr{};
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QUrl            url(urlStr);
    QNetworkRequest req(url);

    std::unique_ptr<QNetworkReply> reply(funcReq(mgr, req));
    // blocks stack until "finished()" has been called
    eventLoop.exec();
    if (reply->error() != QNetworkReply::NoError)
    {
        LOG_ERROR("Error to download data: " + reply->errorString());
    }
    return {reply->readAll(), reply->error() == QNetworkReply::NoError};
}

}

Answer sendGet(const QString& urlStr)
{
    auto funcReq = [](QNetworkAccessManager& mgr, const QNetworkRequest& req)
    { return std::unique_ptr<QNetworkReply>(mgr.get(req)); };

    auto [data, ok] = sendReq(urlStr, funcReq);

    return {data, ok};
}

Answer sendPost(const QString& urlStr, const QUrlQuery& params)
{
    auto funcReq = [&](QNetworkAccessManager& mgr, QNetworkRequest& req)
    {
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        return std::unique_ptr<QNetworkReply>(mgr.post(req, params.query().toUtf8()));
    };

    auto [data, ok] = sendReq(urlStr, funcReq);

    return {data, ok};
}

Answer uploadFile(const QString& fileName, std::unique_ptr<QIODevice> file, const QString& serverUrl)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"file\"; filename=\"" + fileName + "\""));
    QIODevice* filePtr = file.release();
    filePart.setBodyDevice(filePtr);
    // we delegate the deletion of the file to the multiPart
    filePtr->setParent(multiPart);

    multiPart->append(filePart);

    auto funcReq = [multiPart](QNetworkAccessManager& mgr, QNetworkRequest& req) -> std::unique_ptr<QNetworkReply>
    {
        req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + multiPart->boundary());
        auto reply = mgr.post(req, multiPart);
        // Ensure multiPart is deleted with reply
        multiPart->setParent(reply);
        return std::unique_ptr<QNetworkReply>(reply);
    };

    auto [data, ok] = sendReq(serverUrl, funcReq);

    return {data, ok};
}

std::optional<QByteArray> downloadData(const QString& urlStr)
{
    auto funcReq = [](QNetworkAccessManager& mgr, const QNetworkRequest& req)
    { return std::unique_ptr<QNetworkReply>(mgr.get(req)); };

    auto [data, ok] = sendReq(urlStr, funcReq);

    if (!ok)
    {
        return std::nullopt;
    }

    return data;
}
