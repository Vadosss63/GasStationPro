#include <QApplication>
#include <QErrorMessage>
#include "logging.h"

#include "tazs.h"

void setQSS()
{
    qApp->setStyleSheet("ServiceMenuWindow{background:"
                        "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 "
                        "#4287ff, stop: 1 #351ccc);"
                        "}");
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    constexpr auto filePath = "settings.json";

    std::optional<Configure> conf = readConfigure(filePath);
    if (!conf)
    {
        constexpr auto errorMsg = "The settings.json contains invalid fields!";

        std::unique_ptr<QErrorMessage> errorMessage = std::make_unique<QErrorMessage>();
        LOG_ERROR(errorMsg);
        errorMessage->showMessage(errorMsg);
    }

    Tazs tAzs;

    tAzs.run();

    setQSS();
    return a.exec();
}
