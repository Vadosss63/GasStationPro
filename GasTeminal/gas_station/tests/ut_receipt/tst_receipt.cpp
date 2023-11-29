#include <QBuffer>
#include <QCoreApplication>
#include <QtTest>

#include "receipt.h"

namespace
{
QByteArray  buf{};
QStringList fileList{};

void setFileList(const QStringList& list)
{
    fileList = list;
}

void setTestBuf(const QString& str)
{
    buf = str.toUtf8();
}
}

std::unique_ptr<QIODevice> openFile(const QString&, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> buffer = std::make_unique<QBuffer>(&buf);

    buffer->open(mode);
    return buffer;
}

void createDirIfNeeded(const QString& /*dirPath*/) {}

void removeFile(const QString& /*filePath*/) {}

QStringList getDirectoryFileList(const QString& /*dirPath*/)
{
    return fileList;
}
#include <iostream>
class ReceiptTest : public QObject
{
    Q_OBJECT

public:
    ReceiptTest() {}
    ~ReceiptTest() {}

private slots:
    void cleanup()
    {
        buf.clear();
        fileList.clear();
        QVERIFY(buf.isEmpty());
        QVERIFY(fileList.empty());
    }

    void listReciptFilesEmpty();
    void listReciptFilesNoMatches();
    void listReciptFilesMatches();
    void getReceipt();
    void getReceiptJson();
    void receiptFromFile();
    void receiptToFile();
};

void ReceiptTest::listReciptFilesEmpty()
{
    QStringList retFileList = getListReciptFiles();
    QVERIFY(retFileList.empty());
}

void ReceiptTest::listReciptFilesNoMatches()
{
    const QStringList list{"123.", "dad.son", "d321.txt", ".ss"};
    setFileList(list);
    QStringList retFileList = getListReciptFiles();
    QVERIFY(retFileList.empty());
}

void ReceiptTest::listReciptFilesMatches()
{
    const QStringList inputList{"12.json", "dad.son", "d321.json", "1113.ss", "3211.json", "6782.json", "adsadw.json"};
    const QStringList expectedList{"12.json", "3211.json", "6782.json"};
    setFileList(inputList);
    QStringList retFileList = getListReciptFiles();

    QVERIFY(!retFileList.empty());
    QCOMPARE(retFileList, expectedList);
}

void ReceiptTest::getReceipt()
{
    const QString expectedStrReceipt = {"Дата: 29.11.2023 10:00\n"
                                        "Колонка: 1\n"
                                        "Топливо: АИ 92\n"
                                        "Литры: 45 Л\n"
                                        "Сумма: 10230 руб"};

    const Receipt receipt{.time         = 2131231,
                          .date         = "29.11.2023 10:00",
                          .numOfAzsNode = 1,
                          .gasType      = "АИ 92",
                          .countLitres  = "45 Л",
                          .sum          = "10230"};

    const QString retStrReceipt = receipt.getReceipt();
    QCOMPARE(retStrReceipt, expectedStrReceipt);
}

void ReceiptTest::getReceiptJson()
{
    QString expectedJson = R"({
    "count_litres": "22 Л",
    "data": "29.11.2025 12:22",
    "gas_type": "АИ 95",
    "num_azs_node": 2,
    "sum": "10230",
    "time": 2131231
}
)";

    const Receipt receipt{.time         = 2131231,
                          .date         = "29.11.2025 12:22",
                          .numOfAzsNode = 2,
                          .gasType      = "АИ 95",
                          .countLitres  = "22 Л",
                          .sum          = "10230"};

    const QString retStrJson = receipt.getReceiptJson();
    QCOMPARE(retStrJson, expectedJson);
}

void ReceiptTest::receiptFromFile()
{
    const Receipt expectedReceipt{.time         = 2131231,
                                  .date         = "01.11.1999 18:22",
                                  .numOfAzsNode = 2,
                                  .gasType      = "ДТ",
                                  .countLitres  = "840 Л",
                                  .sum          = "45677"};

    QString jsonToWrite = R"({
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(actualReceiptOpt.has_value());

    const Receipt actualReceipt = actualReceiptOpt.value();

    QCOMPARE(actualReceipt.time, expectedReceipt.time);
    QCOMPARE(actualReceipt.date, expectedReceipt.date);
    QCOMPARE(actualReceipt.numOfAzsNode, expectedReceipt.numOfAzsNode);
    QCOMPARE(actualReceipt.gasType, expectedReceipt.gasType);
    QCOMPARE(actualReceipt.countLitres, expectedReceipt.countLitres);
    QCOMPARE(actualReceipt.sum, expectedReceipt.sum);
}

void ReceiptTest::receiptToFile()
{
    QString expectedFileContent = R"({
    "count_litres": "22 Л",
    "data": "29.11.2025 12:22",
    "gas_type": "АИ 95",
    "num_azs_node": 2,
    "sum": "10230",
    "time": 2348320
}
)";

    const Receipt receiptToWrite{.time         = 2348320,
                                 .date         = "29.11.2025 12:22",
                                 .numOfAzsNode = 2,
                                 .gasType      = "АИ 95",
                                 .countLitres  = "22 Л",
                                 .sum          = "10230"};

    writeReceiptToFile(receiptToWrite);

    QString actualFileContent = QString(buf);

    QCOMPARE(actualFileContent, expectedFileContent);
}

QTEST_MAIN(ReceiptTest)

#include "tst_receipt.moc"
