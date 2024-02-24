#include "receipt.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "appsettings.h"
#include "filesystemutilities.h"

QString Receipt::getReceipt() const
{
    return QString("Дата: %1\n"
                   "Колонка: %2\n"
                   "Топливо: %3\n"
                   "Литры: %4\n"
                   "Сумма: %5 руб")
        .arg(date, QString::number(numOfAzsNode), gasType, countLitres, sum);
}

QString Receipt::getReceiptJson() const
{
    const QJsonObject receipt{{"time", time},
                              {"data", date},
                              {"num_azs_node", numOfAzsNode},
                              {"gas_type", gasType},
                              {"count_litres", countLitres},
                              {"sum", sum}};

    QString receiptJson = QString::fromUtf8(QJsonDocument(receipt).toJson());

    return receiptJson;
}

void writeReceiptToFile(const Receipt& receipt)
{
    const QString folderPath = AppSettings::instance().getReceiptFolderName();
    const QString filePath   = QString("%1%2.json").arg(folderPath).arg(receipt.time);

    if (!createDirIfNeeded(folderPath))
    {
        qDebug() << "Failed to create directory: " << folderPath;
        return;
    }

    std::unique_ptr<QIODevice> fileReceipt = openFile(filePath, QIODevice::WriteOnly);
    if (!fileReceipt->isOpen())
    {
        qDebug() << "Failed to write Receipt to json";
        return;
    }

    const std::string dataTowrite{receipt.getReceiptJson().toStdString()};

    fileReceipt->write(dataTowrite.c_str());
    fileReceipt->close();
}

std::optional<Receipt> readReceiptFromFile(const QString& fileReceiptPath)
{
    std::unique_ptr<QIODevice> fileReceipt = openFile(fileReceiptPath, QIODevice::ReadOnly | QIODevice::Text);
    if (!fileReceipt->isOpen())
    {
        qDebug() << "Can not open json file";
        return std::nullopt;
    }

    const QString jsonData = fileReceipt->readAll();
    fileReceipt->close();

    const QJsonDocument doc  = QJsonDocument::fromJson(jsonData.toUtf8());
    const QJsonObject   json = doc.object();

    const QList<QString> requiredFields = {"time", "data", "num_azs_node", "gas_type", "count_litres", "sum"};

    for (const QString& field : requiredFields)
    {
        if (!json.contains(field))
        {
            qDebug() << "Missing or invalid field in settings: " << field;
            return std::nullopt;
        }
    }

    Receipt receipt{.time         = json["time"].toInt(),
                    .date         = json["data"].toString(),
                    .numOfAzsNode = json["num_azs_node"].toInt(),
                    .gasType      = json["gas_type"].toString(),
                    .countLitres  = json["count_litres"].toString(),
                    .sum          = json["sum"].toString()

    };

    return receipt;
}

QStringList getListReciptFiles()
{
    const QString folderPath = AppSettings::instance().getReceiptFolderName();

    const QRegExp     expr{"^\\d{10}\\.json$"};
    const QStringList allFiles{getDirectoryFileList(folderPath)};

    return allFiles.filter(expr);
}
