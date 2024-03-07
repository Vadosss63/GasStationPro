#pragma once

#include <QObject>
#include <QTimer>

#include "webservercontroller.h"

namespace loguploader
{
class AppUpdater : public QObject
{
    Q_OBJECT
public:
    AppUpdater(QObject* parent = nullptr);

    void run();
    void stop();

private slots:

    void pollServer();

private:
    bool downloadFile(const QString& url);

    bool updateApp();

    bool unpackArchive();

    WebServerController webServerController{};
    QTimer              timer{};

    QString storedFileName{};

    static constexpr auto updatePath = "update/";
};

}
