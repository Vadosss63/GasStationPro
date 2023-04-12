#include "receipt.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "appsettings.h"

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
    QJsonObject receipt;
    receipt.insert("time", time);
    receipt.insert("data", date);
    receipt.insert("num_azs_node", numOfAzsNode);
    receipt.insert("gas_type", gasType);
    receipt.insert("count_litres", countLitres);
    receipt.insert("sum", sum);

    QString receiptJson = QString::fromUtf8(QJsonDocument(receipt).toJson());
    return receiptJson;
}

void writeReceiptToFile(const Receipt& receipt)
{
    QString path = AppSettings::instance().getReceiptFolderName();
    QFile   file = QString(path + "%1.json").arg(receipt.time);
    QDir    dir;

    if (!dir.exists(path))
    {
        dir.mkdir(path);
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to write Receipt to json";
        return;
    }

    file.write(receipt.getReceiptJson().toStdString().c_str());
    file.close();
}
