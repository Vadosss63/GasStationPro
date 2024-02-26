#include "httprequest.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <functional>
#include <memory>

Answer sendReq(const QString&                                                                          urlStr,
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

    Answer answer{reply->readAll(), reply->error() == QNetworkReply::NoError};
    return answer;
}

Answer sendGet(const QString& urlStr)
{
    return sendReq(urlStr,
                   [](QNetworkAccessManager& mgr, const QNetworkRequest& req)
                   { return std::unique_ptr<QNetworkReply>(mgr.get(req)); });
}

Answer sendPost(const QString& urlStr, const QUrlQuery& params)
{
    return sendReq(urlStr,
                   [&](QNetworkAccessManager& mgr, QNetworkRequest& req)
                   {
                       req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                       return std::unique_ptr<QNetworkReply>(mgr.post(req, params.query().toUtf8()));
                   });
}
