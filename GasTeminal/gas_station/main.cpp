#include <QApplication>

#include "dialogmain.h"

void loadQSS()
{
    qApp->setStyleSheet("SettingWindows{background:"
                        "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 "
                        "#4287ff, stop: 1 #351ccc);"
                        "}");
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    DialogMain w;
    w.showFullScreen();
    loadQSS();
    return a.exec();
}
