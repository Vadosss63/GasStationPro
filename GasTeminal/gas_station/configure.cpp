#include "configure.h"

bool readConfigure(const QString& fileName, Configure& configure)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file!";
        return false;
    }
    QByteArray jsonData = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);

    if (document.isNull()) {
        qDebug() << "Failed to parse JSON!";
        return false;
    }

    QJsonObject object = document.object();

    if (object.isEmpty() || !object.contains("Id") || !object.contains("Name")
        || !object.contains("Address") || !object.contains("Token")
        || !object.contains("Host") || !object.contains("PhoneOfSupport")
        || !object.contains("ActiveBtn2") || !object.contains("ComPort")
        || !object.contains("BaudRate")) {
        qDebug() << "Missing or invalid field(s) in settings!";
        return false;
    }

    configure.id = object["Id"].toString();
    configure.name = object["Name"].toString();
    configure.address = object["Address"].toString();
    configure.token = object["Token"].toString();
    configure.host = object["Host"].toString();
    configure.phoneOfSupport = object["PhoneOfSupport"].toString();
    configure.activeBtn2 = object["ActiveBtn2"].toInt();
    configure.comPort = object["ComPort"].toString();
    configure.baudRate = object["BaudRate"].toInt();

    return true;
}
