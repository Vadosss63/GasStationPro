// clang-off
#include <QBuffer>
// clang-on

#include <CppUTest/TestHarness.h>

#include <CppUTestExt/MockSupport.h>

#include <QStringList>
#include <string_view>

#include "appsettings.h"

namespace
{
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

TEST_GROUP (AppsettingsTest)
{
protected:
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
};

TEST (AppsettingsTest, testParams)
{
    AppSettings& settings = AppSettings::instance();
    CHECK_EQUAL(QString("./receiptCache/"), settings.getReceiptFolderName());

    AppSettings::Settings& appSettings = settings.getSettings();
    CHECK_EQUAL(0, appSettings.sum);
}

TEST (AppsettingsTest, testReadOneLogFile)
{
    QByteArray receipt = "Дата: 21.07.2021 22:54\n"
                         "Колонка: 1\n"
                         "Бензин: АИ 92\n"
                         "Литры: 20 Л\n"
                         "Сумма: 1000.00₽ руб";

    constexpr std::string_view expectedLogsName{"logs.log"};
    expectOpenFile(expectedLogsName, QIODevice::ReadOnly);
    expectSize(receipt.size());
    expectReadData(receipt.data(), receipt.size());

    QStringList   logs            = AppSettings::instance().readLogFile();
    constexpr int expectedLogSize = 1;
    CHECK_EQUAL(expectedLogSize, logs.size());
    CHECK_TRUE(logs.contains(receipt));
}

TEST (AppsettingsTest, testReadTwoLogFiles)
{
    QString expectedReceipt1 = "Дата: 21.07.2021 22:54\n"
                               "Колонка: 1\n"
                               "Бензин: АИ 92\n"
                               "Литры: 20 Л\n"
                               "Сумма: 1000.00₽ руб";

    QString expectedReceipt2 = "Дата: 22.07.2021 22:54\n"
                               "Колонка: 1\n"
                               "Бензин: АИ 92\n"
                               "Литры: 20 Л\n"
                               "Сумма: 2000.00₽ руб";

    QByteArray receipts = "Дата: 21.07.2021 22:54\n"
                          "Колонка: 1\n"
                          "Бензин: АИ 92\n"
                          "Литры: 20 Л\n"
                          "Сумма: 1000.00₽ руб"
                          "\n####\n"
                          "Дата: 22.07.2021 22:54\n"
                          "Колонка: 1\n"
                          "Бензин: АИ 92\n"
                          "Литры: 20 Л\n"
                          "Сумма: 2000.00₽ руб";

    constexpr std::string_view expectedLogsName{"logs.log"};
    expectOpenFile(expectedLogsName, QIODevice::ReadOnly);
    expectSize(receipts.size());
    expectReadData(receipts.data(), receipts.size());

    AppSettings&  settings        = AppSettings::instance();
    QStringList   logs            = settings.readLogFile();
    constexpr int expectedLogSize = 2;
    CHECK_EQUAL(logs.size(), expectedLogSize);
    CHECK_TRUE(logs.contains(expectedReceipt1));
    CHECK_TRUE(logs.contains(expectedReceipt2));
}

TEST (AppsettingsTest, testAddOneLogFile)
{
    QString receipt = "Дата: 21.07.2021 22:54\n"
                      "Колонка: 1\n"
                      "Бензин: АИ 92\n"
                      "Литры: 20 Л\n"
                      "Сумма: 1000.00₽ руб";

    const std::string_view expectedReceipt = "Дата: 21.07.2021 22:54\n"
                                             "Колонка: 1\n"
                                             "Бензин: АИ 92\n"
                                             "Литры: 20 Л\n"
                                             "Сумма: 1000.00₽ руб"
                                             "\n####\n";

    constexpr std::string_view expectedLogsName{"logs.log"};
    expectOpenFile(expectedLogsName, QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    expectWriteData(expectedReceipt, expectedReceipt.size());

    AppSettings::instance().addTextToLogFile(receipt);
}
