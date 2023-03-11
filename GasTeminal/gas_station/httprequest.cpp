#include "httprequest.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <functional>

Answer sendReq(const QString& urlStr, std::function<QNetworkReply*(QNetworkAccessManager&, QNetworkRequest&)> funcReq)
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QUrl            url(urlStr);
    QNetworkRequest req(url);

    QNetworkReply* reply = funcReq(mgr, req);

    eventLoop.exec(); // blocks stack until "finished()" has been called

    Answer answer{reply->readAll(), reply->error() == QNetworkReply::NoError};
    delete reply;
    return answer;
}

Answer sendGet(const QString& urlStr)
{
    return sendReq(urlStr, [](QNetworkAccessManager& mgr, const QNetworkRequest& req) { return mgr.get(req); });
}

Answer sendPost(const QString& urlStr, const QUrlQuery& params)
{
    return sendReq(urlStr,
                   [&](QNetworkAccessManager& mgr, QNetworkRequest& req)
                   {
                       req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                       return mgr.post(req, params.query().toUtf8());
                   });
}
