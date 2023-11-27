#include <QBuffer>
#include <QCoreApplication>
#include <QtTest>

#include "receipt.h"

namespace
{
QByteArray buf{};
}

std::unique_ptr<QIODevice> openFile(const QString&, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> buffer = std::make_unique<QBuffer>(&buf);

    buffer->open(mode);
    return buffer;
}

class ReceiptTest : public QObject
{
    Q_OBJECT

public:
    ReceiptTest() {}
    ~ReceiptTest() {}

private slots:
    void initTestCase() { }
    void cleanupTestCase() {}

    void testParams();
};

void ReceiptTest::testParams()
{
//    QString fileContent = R"({
//        "Id": "10111920",
//        "Name": "Автозаправка-1",
//        "Address": "СПб, Петроградка 28",
//        "Token": "ef4cfcf144999ed560e9f9ad20000000",
//        "Host": "http://t-azs.ru",
//        "PhoneOfSupport": "8(904)000-00-00",
//        "ActiveBtn2": 2,
//        "ComPort": "/dev/ttyS4",
//        "BaudRate": 115200,
//        "ShowSecondPrice": 1
//    })";
//    setTestBuf(fileContent);

//    Configure conf;
//    bool      isOk = readConfigure("", conf);
//    QVERIFY(isOk);

//    QCOMPARE(conf.id, "10111920");
//    QCOMPARE(conf.name, "Автозаправка-1");
//    QCOMPARE(conf.token, "ef4cfcf144999ed560e9f9ad20000000");
//    QCOMPARE(conf.host, "http://t-azs.ru");
//    QCOMPARE(conf.phoneOfSupport, "8(904)000-00-00");
//    QCOMPARE(conf.activeBtn2, 2);
//    QCOMPARE(conf.comPort, "/dev/ttyS4");
//    QCOMPARE(conf.baudRate, 115200);
//    QCOMPARE(conf.showSecondPrice, 1);
}



QTEST_MAIN(ReceiptTest)

#include "tst_receipt.moc"
