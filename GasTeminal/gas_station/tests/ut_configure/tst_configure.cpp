#include <QBuffer>
#include <QCoreApplication>
#include <QtTest>

#include "configure.h"

namespace
{
QByteArray buf{};

void setTestBuf(const QString& str)
{
    buf = str.toUtf8();
}
}

std::unique_ptr<QIODevice> openFile(const QString&, QIODevice::OpenMode)
{
    std::unique_ptr<QIODevice> buffer = std::make_unique<QBuffer>(&buf);

    buffer->open(QIODevice::ReadWrite);
    return buffer;
}

class ConfigureTest : public QObject
{
    Q_OBJECT

public:
    ConfigureTest() {}
    ~ConfigureTest() {}

private slots:
    void initTestCase() { setTestBuf(""); }
    void cleanupTestCase() {}

    void testParams();
    void testMissedId();
    void testMissedName();
    void testMissedToken();
    void testMissedHost();
    void testMissedPhone();
    void testMissedActiveBtn2();
    void testMissedComPort();
    void testMissedBaudRate();
    void testMissedShowSecondPrice();

private:
    void checkMissedParams()
    {
        Configure conf;
        bool      isOk = readConfigure("", conf);
        QVERIFY(!isOk);
    }
};

void ConfigureTest::testParams()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    Configure conf;
    bool      isOk = readConfigure("", conf);
    QVERIFY(isOk);

    QCOMPARE(conf.id, "10111920");
    QCOMPARE(conf.name, "Автозаправка-1");
    QCOMPARE(conf.token, "ef4cfcf144999ed560e9f9ad20000000");
    QCOMPARE(conf.host, "http://t-azs.ru");
    QCOMPARE(conf.phoneOfSupport, "8(904)000-00-00");
    QCOMPARE(conf.activeBtn2, 2);
    QCOMPARE(conf.comPort, "/dev/ttyS4");
    QCOMPARE(conf.baudRate, 115200);
    QCOMPARE(conf.showSecondPrice, 1);
}

void ConfigureTest::testMissedId()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedName()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedToken()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedHost()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedPhone()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedActiveBtn2()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedComPort()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedBaudRate()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

void ConfigureTest::testMissedShowSecondPrice()
{
    QString fileContent = R"({
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
    setTestBuf(fileContent);

    checkMissedParams();
}

QTEST_MAIN(ConfigureTest)

#include "tst_configure.moc"
