#include <QCoreApplication>

#include "appupdater.h"
#include "configure.h"
#include "logging.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    constexpr auto filePath = "settings.json";

    std::optional<Configure> conf = readConfigure(filePath);
    if (!conf)
    {
        LOG_ERROR("The settings.json contains invalid fields!");
        return EXIT_FAILURE;
    }

    loguploader::AppUpdater appUpdater(std::move(*conf));
    appUpdater.run();
    return a.exec();
}
