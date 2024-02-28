#include <QCoreApplication>

#include "loguploader.h"

int main(int argc, char* argv[])
{
    QCoreApplication         a(argc, argv);
    loguploader::LogUploader logUploader;
    logUploader.run();
    return a.exec();
}
