#include <QCoreApplication>

#include "appupdater.h"
#include "filesystemutilities.h"
#include "httprequest.h"
#include "logging.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    loguploader::AppUpdater appUpdater;
    appUpdater.run();
    return a.exec();
}
