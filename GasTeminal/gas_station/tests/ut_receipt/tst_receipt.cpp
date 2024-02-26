#include <QBuffer>
#include <QCoreApplication>
#include <QtTest>

#include "receipt.h"

namespace
{
QByteArray  buf{};
QStringList fileList{};

QString expectedFilePath{"./receiptCache/"};

QString actualCreatedDir{};
QString actualOpenedFile{};
QString actualFilePathForList{};

void setExpectedFileList(const QStringList& list)
{
    fileList = list;
}

void setExpectedTestBuf(const QString& str)
{
    buf = str.toUtf8();
}
}

std::unique_ptr<QIODevice> openFile(const QString& filePath, QIODevice::OpenMode mode)
{
    actualOpenedFile                  = filePath;
    std::unique_ptr<QIODevice> buffer = std::make_unique<QBuffer>(&buf);

    buffer->open(mode);
    return buffer;
}

bool createDirIfNeeded(const QString& dirPath)
{
    actualCreatedDir = dirPath;
    return true;
}

void removeFile(const QString& /*filePath*/) {}

QStringList getDirectoryFileList(const QString& dirPath)
{
    actualFilePathForList = dirPath;
    return fileList;
}

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
        actualCreatedDir.clear();
        actualOpenedFile.clear();
        actualFilePathForList.clear();
        QVERIFY(buf.isEmpty());
        QVERIFY(fileList.empty());
        QVERIFY(actualCreatedDir.isEmpty());
        QVERIFY(actualOpenedFile.isEmpty());
        QVERIFY(actualFilePathForList.isEmpty());
    }

    void listReciptFilesEmpty();
    void listReciptFilesNoMatches();
    void listReciptFilesMatches();
    void getReceipt();
    void getReceiptJson();
    void receiptToFile();
    void receiptFromFile();
    void receiptFromFileEmpty();
    void receiptFromFileMissedTime();
    void receiptFromFileMissedSum();
    void receiptFromFileMissedNumAzs();
    void receiptFromFileMissedGasType();
    void receiptFromFileMissedData();
    void receiptFromFileMissedLiters();
    void receiptFromFileMissedCashless();
    void receiptFromFileMissedCash();
};

void ReceiptTest::listReciptFilesEmpty()
{
    QStringList retFileList = getListReciptFiles();
    QCOMPARE(actualFilePathForList, expectedFilePath);
    QVERIFY(retFileList.empty());
}

void ReceiptTest::listReciptFilesNoMatches()
{
    const QStringList list{"123.", "dad.son", "d321.txt", ".ss", "17019122121.json"};
    setExpectedFileList(list);
    QStringList retFileList = getListReciptFiles();
    QCOMPARE(actualFilePathForList, expectedFilePath);
    QVERIFY(retFileList.empty());
}

void ReceiptTest::listReciptFilesMatches()
{
    const QStringList inputList{"1701612275.json",
                                "dad.son",
                                "17016122.json",
                                "1113.ss",
                                "3211.json",
                                "1701912212.json",
                                "adsadw.json",
                                "17019122121.json"};
    const QStringList expectedList{"1701612275.json", "1701912212.json"};
    setExpectedFileList(inputList);
    QStringList retFileList = getListReciptFiles();

    QCOMPARE(actualFilePathForList, expectedFilePath);
    QVERIFY(!retFileList.empty());
    QCOMPARE(retFileList, expectedList);
}

void ReceiptTest::getReceipt()
{
    const QString expectedStrReceipt = {"Дата: 29.11.2023 10:00\n"
                                        "Колонка: 1\n"
                                        "Топливо: АИ 92\n"
                                        "Литры: 45 Л\n"
                                        "Безнал: 230.00\n"
                                        "Налич: 1000.00\n"
                                        "Сумма: 10230 руб"};

    const Receipt receipt{.time         = 2131231,
                          .date         = "29.11.2023 10:00",
                          .numOfAzsNode = 1,
                          .gasType      = "АИ 92",
                          .countLitres  = "45 Л",
                          .cash         = 1000.0,
                          .cashless     = 230.0,
                          .sum          = "10230"};

    const QString retStrReceipt = receipt.getReceipt();
    QCOMPARE(retStrReceipt, expectedStrReceipt);
}

void ReceiptTest::getReceiptJson()
{
    QString expectedJson = R"({
    "cash": 1000.67,
    "cashless": 230.56,
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
                          .cash         = 1000.67,
                          .cashless     = 230.56,
                          .sum          = "10230"};

    const QString retStrJson = receipt.getReceiptJson();
    QCOMPARE(retStrJson, expectedJson);
}

void ReceiptTest::receiptToFile()
{
    QString expectedFileContent = R"({
    "cash": 1000,
    "cashless": 230,
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
                                 .cash         = 1000,
                                 .cashless     = 230,
                                 .sum          = "10230"};

    writeReceiptToFile(receiptToWrite);

    QString actualFileContent = QString(buf);

    QCOMPARE(actualCreatedDir, "./receiptCache/");
    QCOMPARE(actualOpenedFile, "./receiptCache/2348320.json");
    QCOMPARE(actualFileContent, expectedFileContent);
}

void ReceiptTest::receiptFromFile()
{
    const Receipt expectedReceipt{.time         = 2131231,
                                  .date         = "01.11.1999 18:22",
                                  .numOfAzsNode = 2,
                                  .gasType      = "ДТ",
                                  .countLitres  = "840 Л",
                                  .cash         = 1000,
                                  .cashless     = 230,
                                  .sum          = "45677"};

    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(actualReceiptOpt.has_value());

    const Receipt actualReceipt = actualReceiptOpt.value();

    QCOMPARE(actualReceipt.time, expectedReceipt.time);
    QCOMPARE(actualReceipt.date, expectedReceipt.date);
    QCOMPARE(actualReceipt.numOfAzsNode, expectedReceipt.numOfAzsNode);
    QCOMPARE(actualReceipt.gasType, expectedReceipt.gasType);
    QCOMPARE(actualReceipt.countLitres, expectedReceipt.countLitres);
    QCOMPARE(actualReceipt.cash, expectedReceipt.cash);
    QCOMPARE(actualReceipt.cashless, expectedReceipt.cashless);
    QCOMPARE(actualReceipt.sum, expectedReceipt.sum);
}

void ReceiptTest::receiptFromFileEmpty()
{
    setExpectedTestBuf("");
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedTime()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedSum()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedNumAzs()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedGasType()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedData()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": "840 Л",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedLiters()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedCash()
{
    QString jsonToWrite = R"({
        "cashless": 230,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

void ReceiptTest::receiptFromFileMissedCashless()
{
    QString jsonToWrite = R"({
        "cash": 1000,
        "count_litres": "840 Л",
        "data": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": "45677",
        "time": 2131231
    })";

    setExpectedTestBuf(jsonToWrite);
    const auto actualReceiptOpt = readReceiptFromFile("");

    QVERIFY(!actualReceiptOpt.has_value());
}

QTEST_MAIN(ReceiptTest)

#include "tst_receipt.moc"
