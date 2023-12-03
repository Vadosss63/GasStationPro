#pragma once

#include <QFile>
#include <QString>
#include <optional>

struct Receipt
{
    int     time;
    QString date;
    int     numOfAzsNode;
    QString gasType;
    QString countLitres;
    QString sum;

    QString getReceipt() const;
    QString getReceiptJson() const;
};

void                   writeReceiptToFile(const Receipt& receipt);
std::optional<Receipt> readReceiptFromFile(const QString& fileReceiptPath);
QStringList            getListReciptFiles();
