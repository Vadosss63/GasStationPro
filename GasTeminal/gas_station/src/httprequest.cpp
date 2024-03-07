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
