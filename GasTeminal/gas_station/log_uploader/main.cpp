#include <QCoreApplication>
#include <cstdlib>

#include "configure.h"
#include "logging.h"
#include "loguploader.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    constexpr auto           filePath = "settings.json";
    std::optional<Configure> conf     = readConfigure(filePath);
    if (!conf)
    {
        LOG_ERROR("The settings.json contains invalid fields!");
        return EXIT_FAILURE;
    }

    loguploader::LogUploader logUploader(std::move(*conf));
    logUploader.run();
    return a.exec();
}
