#include "loguploader.h"

#include "filesystemutilities.h"
#include "logging.h"
#include "webservercontroller.h"

namespace loguploader
{
LogUploader::LogUploader() {}

void LogUploader::run()
{
    // выполняем опрос сервера 1 раз 10 сек
    // если пришел запрос на сохранения логов выполняем архивацию
    //

    constexpr auto folderPath  = "/home/vadosss63/.GasStationPro/azs_logs";
    constexpr auto archivePath = "/home/vadosss63/.GasStationPro/azsLogs.tar.gz";
    if (archiveFolder(folderPath, archivePath))
    {
        LOG_INFO("Ok");
        WebServerController webServerController;
        QString             serverUrl = "https://vadosss-dev.ru:8080/upload";

        if (webServerController.sendLogsToServer(archivePath, serverUrl))
        {
            LOG_INFO(QString("%1 uploaded").arg(archivePath));
        }
        return;
    }
    LOG_INFO("NOk");
}
}
