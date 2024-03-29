// clang-off
#include <QBuffer>
// clang-on

#include <CppUTest/TestHarness.h>

#include <CppUTestExt/MockSupport.h>

#include <QStringList>
#include <string_view>

#include "receipt.h"

namespace
{
QStringList                fileList{};
constexpr std::string_view expectedDir{"./receiptCache/"};

void setDirectoryFileList(QStringList list)
{
    fileList = std::move(list);
}

SimpleString StringFrom(const QString& str)
{
    return str.toStdString().c_str();
}
}

class MockFileDevice : public QBuffer
{
public:
    using QBuffer::QBuffer;

    qint64 size() const override
    {
        return mock("MockFileDevice").actualCall("size").returnLongLongIntValueOrDefault(0);
    }

    qint64 readData(char* data, qint64 maxlen) override
    {
        return mock("MockFileDevice")
            .actualCall("readData")
            .withOutputParameter("data", data)
            .withParameter("maxlen", maxlen)
            .returnLongLongIntValueOrDefault(0);
    }

    void setMode(QIODevice::OpenMode mode) { QIODevice::setOpenMode(mode); }

    qint64 writeData(const char* data, qint64 len) override
    {
        return mock("MockFileDevice")
            .actualCall("writeData")
            .withStringParameter("data", data)
            .withParameter("len", len)
            .returnLongLongIntValueOrDefault(0);
    }
};

bool createDirIfNeeded(const QString& dirPath)
{
    return mock("filesystemutilities")
        .actualCall("createDirIfNeeded")
        .withStringParameter("dirPath", dirPath.toStdString().c_str())
        .returnBoolValueOrDefault(true);
}

QStringList getDirectoryFileList(const QString& dirPath)
{
    const std::string pathStd = dirPath.toStdString();
    mock("filesystemutilities").actualCall("getDirectoryFileList").withStringParameter("dirPath", pathStd.c_str());
    return fileList;
}

std::unique_ptr<QIODevice> openFile(const QString& path, QIODevice::OpenMode mode)
{
    mock("filesystemutilities")
        .actualCall("openFile")
        .withStringParameter("dirPath", path.toStdString().c_str())
        .withParameter("OpenMode", mode);

    auto device = std::make_unique<MockFileDevice>();
    device->setMode(mode);
    return device;
}

TEST_GROUP (ReceiptTest)
{
    void setup()
    {
        fileList.clear();
    }

protected:
    static void expectCreateDirIfNeeded(const std::string_view expectedDirPath, bool retValue = true)
    {
        mock("filesystemutilities")
            .expectOneCall("createDirIfNeeded")
            .withStringParameter("dirPath", expectedDirPath.data())
            .andReturnValue(retValue);
    }

    static void expectGetDirectoryFileList()
    {
        mock("filesystemutilities")
            .expectOneCall("getDirectoryFileList")
            .withStringParameter("dirPath", "./receiptCache/");
    }

    static void expectWriteData(const std::string_view expectedData, qint64 retValue = 0)
    {
        mock("MockFileDevice")
            .expectOneCall("writeData")
            .withStringParameter("data", expectedData.data())
            .withParameter("len", expectedData.size())
            .andReturnValue(retValue);
    }

    static void expectOpenFile(const std::string_view expectedPath, QIODevice::OpenMode expectedMmode)
    {
        mock("filesystemutilities")
            .expectOneCall("openFile")
            .withStringParameter("dirPath", expectedPath.data())
            .withParameter("OpenMode", expectedMmode);
    }

    static void expectSize(qint64 retValue = 0)
    {
        mock("MockFileDevice").expectOneCall("size").andReturnValue(retValue);
    }

    static void expectReadData(char* data, qint64 retValue)
    {
        mock("MockFileDevice")
            .expectOneCall("readData")
            .withOutputParameterReturning("data", data, retValue)
            .ignoreOtherParameters()
            .andReturnValue(retValue);
    }

    static void expectReadFromFile(QByteArray & jsonToWrite, const std::string_view expectedFilePath)
    {
        expectOpenFile(expectedFilePath, QIODevice::ReadOnly | QIODevice::Text);
        expectSize(jsonToWrite.size());
        expectReadData(jsonToWrite.data(), jsonToWrite.size());
    }
};

TEST (ReceiptTest, listReciptFilesEmpty)
{
    expectGetDirectoryFileList();

    QStringList retFileList = getListReciptFiles();
    CHECK_TRUE(retFileList.empty());
}

TEST (ReceiptTest, listReciptFilesNoMatches)
{
    expectGetDirectoryFileList();

    const QStringList list{"123.", "dad.son", "d321.txt", ".ss", "17019122121.json"};
    setDirectoryFileList(std::move(list));

    QStringList retFileList = getListReciptFiles();
    CHECK_TRUE(retFileList.empty());
}

TEST (ReceiptTest, listReciptFilesMatches)
{
    expectGetDirectoryFileList();

    const QStringList inputList{"1701612275.json",
                                "dad.son",
                                "17016122.json",
                                "1113.ss",
                                "3211.json",
                                "1701912212.json",
                                "adsadw.json",
                                "17019122121.json"};
    const QStringList expectedList{"1701612275.json", "1701912212.json"};
    setDirectoryFileList(std::move(inputList));

    QStringList retFileList = getListReciptFiles();

    CHECK_FALSE(retFileList.empty());
    CHECK_TRUE(expectedList == retFileList);
}

TEST (ReceiptTest, getReceipt)
{
    const QString expectedStrReceipt = {"Дата: 29.11.2023 10:00\n"
                                        "Колонка: 1\n"
                                        "Топливо: АИ 92\n"
                                        "Литры: 45.00\n"
                                        "Безнал: 230.00\n"
                                        "Налич: 1000.00\n"
                                        "Онлайн: 100.00\n"
                                        "Сумма: 10330.00 руб"};

    const Receipt receipt{.time         = 2131231,
                          .date         = "29.11.2023 10:00",
                          .numOfAzsNode = 1,
                          .gasType      = "АИ 92",
                          .countLitres  = 45,
                          .cash         = 1000.0,
                          .cashless     = 230.0,
                          .online       = 100.0,
                          .sum          = 10330};

    const QString retStrReceipt = receipt.getReceipt();

    CHECK_FALSE(retStrReceipt.isEmpty());
    CHECK_EQUAL(expectedStrReceipt, retStrReceipt);
}

TEST (ReceiptTest, getReceiptJson)
{
    QString expectedJson = R"({
    "cash": 1000.67,
    "cashless": 230.56,
    "count_litres": 22,
    "date": "29.11.2025 12:22",
    "gas_type": "АИ 95",
    "num_azs_node": 2,
    "online": 100,
    "sum": 10230,
    "time": 2131231
}
)";

    const Receipt receipt{.time         = 2131231,
                          .date         = "29.11.2025 12:22",
                          .numOfAzsNode = 2,
                          .gasType      = "АИ 95",
                          .countLitres  = 22,
                          .cash         = 1000.67,
                          .cashless     = 230.56,
                          .online       = 100.00,
                          .sum          = 10230};

    const QString retStrJson = receipt.getReceiptJson();
    CHECK_EQUAL(expectedJson, retStrJson);
}

TEST (ReceiptTest, failedToCreateDir)
{
    expectCreateDirIfNeeded(expectedDir, false);

    const Receipt receiptToWrite{.time         = 2348320,
                                 .date         = "29.11.2025 12:22",
                                 .numOfAzsNode = 2,
                                 .gasType      = "АИ 95",
                                 .countLitres  = 22,
                                 .cash         = 1000,
                                 .cashless     = 230,
                                 .online       = 100.0,
                                 .sum          = 10230};
    writeReceiptToFile(receiptToWrite);
}

TEST (ReceiptTest, receiptToFile)
{
    std::string_view           expectedFileContent = R"({
    "cash": 1000,
    "cashless": 230,
    "count_litres": 22,
    "date": "29.11.2025 12:22",
    "gas_type": "АИ 95",
    "num_azs_node": 2,
    "online": 100,
    "sum": 10230,
    "time": 2348320
}
)";
    constexpr std::string_view expectedFilePath{"./receiptCache/2348320.json"};
    expectCreateDirIfNeeded(expectedDir);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectWriteData(expectedFileContent, expectedFileContent.size());

    const Receipt receiptToWrite{.time         = 2348320,
                                 .date         = "29.11.2025 12:22",
                                 .numOfAzsNode = 2,
                                 .gasType      = "АИ 95",
                                 .countLitres  = 22,
                                 .cash         = 1000,
                                 .cashless     = 230,
                                 .online       = 100,
                                 .sum          = 10230};
    writeReceiptToFile(receiptToWrite);
}

TEST (ReceiptTest, receiptFromFile)
{
    const Receipt expectedReceipt{.time         = 2131231,
                                  .date         = "01.11.1999 18:22",
                                  .numOfAzsNode = 2,
                                  .gasType      = "ДТ",
                                  .countLitres  = 840,
                                  .cash         = 1000,
                                  .cashless     = 230,
                                  .online       = 100,
                                  .sum          = 45677};

    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{"/file/path"};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("/file/path");

    CHECK_TRUE(actualReceiptOpt.has_value());

    const Receipt& actualReceipt = *actualReceiptOpt;

    CHECK_EQUAL(actualReceipt.time, expectedReceipt.time);
    CHECK_EQUAL(actualReceipt.date, expectedReceipt.date);
    CHECK_EQUAL(actualReceipt.numOfAzsNode, expectedReceipt.numOfAzsNode);
    CHECK_EQUAL(actualReceipt.gasType, expectedReceipt.gasType);
    CHECK_EQUAL(actualReceipt.countLitres, expectedReceipt.countLitres);
    CHECK_EQUAL(actualReceipt.cash, expectedReceipt.cash);
    CHECK_EQUAL(actualReceipt.cashless, expectedReceipt.cashless);
    CHECK_EQUAL(actualReceipt.online, expectedReceipt.online);
    CHECK_EQUAL(actualReceipt.sum, expectedReceipt.sum);
}

TEST (ReceiptTest, receiptFromFileEmpty)
{
    QByteArray jsonToWrite{""};

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedTime)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
    })";

    constexpr std::string_view expectedFilePath{"/path/"};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("/path/");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedSum)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{"/path/1/"};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("/path/1/");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedNumAzs)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{"/path/path/1/"};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("/path/path/1/");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedGasType)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedDate)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "count_litres": 840,
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedLiters)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "cashless": 230,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedCash)
{
    QByteArray jsonToWrite = R"({
        "cashless": 230,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedCashless)
{
    QByteArray jsonToWrite = R"({
        "cash": 1000,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "online": 100,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}

TEST (ReceiptTest, receiptFromFileMissedOnline)
{
    QByteArray jsonToWrite = R"({
        "cashless": 230,
        "cash": 1000,
        "count_litres": 840,
        "date": "01.11.1999 18:22",
        "gas_type": "ДТ",
        "num_azs_node": 2,
        "sum": 45677,
        "time": 2131231
    })";

    constexpr std::string_view expectedFilePath{""};
    expectReadFromFile(jsonToWrite, expectedFilePath);

    const auto actualReceiptOpt = readReceiptFromFile("");

    CHECK_FALSE(actualReceiptOpt.has_value());
}
