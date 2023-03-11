#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QUrlQuery>

struct Answer
{
    QString msg = "";
    bool isOk = false;
};

Answer sendGet(const QString& urlStr);

Answer sendPost(const QString& urlStr, const QUrlQuery& params);

#endif // HTTPREQUEST_H
