#include <QApplication>
#include <QThread>

#include "logging.h"
#include "mainwindowcontroller.h"

void setQSS()
{
    qApp->setStyleSheet("ServiceMenuWindow{background:"
                        "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 "
                        "#4287ff, stop: 1 #351ccc);"
                        "}");
}

int main(int argc, char* argv[])
{
    for (size_t i = 0; i < 100; ++i)
    {
        //        QString data = QString("Iteration: %1").arg(i + 1);
        //        printLog((i % 2) ? LogLevel::INFO : LogLevel::ERROR, data);
        LOG_DEBUG("It is a Debug print");
        LOG_INFO("It is a INFO print");
        LOG_WARNING("It is a Warning print");
        LOG_ERROR("It is a ERROR print");
    }
    QApplication a(argc, argv);

    MainWindowController controller;
    controller.showMainWindow();
    setQSS();
    return a.exec();
}
