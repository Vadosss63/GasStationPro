// clang-off
#include <QBuffer>
// clang-on

#include <CppUTest/TestHarness.h>

#include <CppUTestExt/MockSupport.h>

#include <optional>
#include <string_view>

#include "configure.h"

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

TEST_GROUP (ConfigureTest)
{
protected:
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

    static void expectReadFromFile(QByteArray & fileContent, const std::string_view expectedFilePath)
    {
        expectOpenFile(expectedFilePath, QIODevice::ReadOnly | QIODevice::Text);
        expectSize(fileContent.size());
        expectReadData(fileContent.data(), fileContent.size());
    }
};

TEST (ConfigureTest, testParams)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{"/path/to"};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("/path/to");
    CHECK_TRUE(confOpt);
    const Configure& conf = *confOpt;

    CHECK_EQUAL(conf.id, "10111920");
    CHECK_EQUAL(conf.name, "Автозаправка-1");
    CHECK_EQUAL(conf.token, "ef4cfcf144999ed560e9f9ad20000000");
    CHECK_EQUAL(conf.host, "http://t-azs.ru");
    CHECK_EQUAL(conf.phoneOfSupport, "8(904)000-00-00");
    CHECK_EQUAL(conf.activeBtn2, 2);
    CHECK_EQUAL(conf.comPort, "/dev/ttyS4");
    CHECK_EQUAL(conf.baudRate, 115200);
    CHECK_EQUAL(conf.showSecondPrice, 1);
}

TEST (ConfigureTest, testMissedId)
{
    QByteArray fileContent = R"({
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{"/to"};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("/to");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedName)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{""};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedToken)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{""};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedHost)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{""};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedPhone)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{""};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedActiveBtn2)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{"/"};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("/");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedComPort)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "BaudRate": 115200,
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{"/path/"};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("/path/");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedBaudRate)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "ShowSecondPrice": 1
    })";

    constexpr std::string_view expectedFileName{"/11/"};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("/11/");
    CHECK_FALSE(confOpt);
}

TEST (ConfigureTest, testMissedShowSecondPrice)
{
    QByteArray fileContent = R"({
        "Id": "10111920",
        "Name": "Автозаправка-1",
        "Address": "СПб, Петроградка 28",
        "Token": "ef4cfcf144999ed560e9f9ad20000000",
        "Host": "http://t-azs.ru",
        "PhoneOfSupport": "8(904)000-00-00",
        "ActiveBtn2": 2,
        "ComPort": "/dev/ttyS4",
        "BaudRate": 115200,
    })";

    constexpr std::string_view expectedFileName{"/dir/"};
    expectReadFromFile(fileContent, expectedFileName);

    auto confOpt = readConfigure("/dir/");
    CHECK_FALSE(confOpt);
}
