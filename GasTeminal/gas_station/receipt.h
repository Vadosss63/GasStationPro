#pragma once

#include <QFile>
#include <QString>

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

void        writeReceiptToFile(const Receipt& receipt);
bool        readReceiptFromFile(QFile& fileReceipt, Receipt& receipt);
QStringList getListReciptFiles();
