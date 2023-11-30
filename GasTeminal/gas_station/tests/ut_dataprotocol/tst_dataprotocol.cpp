#include <QtTest>

#include "dataprotocol.h"

class DataProtocolTest : public QObject
{
    Q_OBJECT
public:
private slots:
    //    void initTestCase() {}
    //    void cleanupTestCase() {}

    void testStub();
};

void DataProtocolTest::testStub()
{
    bool isOk = true;
    QVERIFY(isOk);
    int expectedVal = 2;
    int val         = 2;
    QCOMPARE(val, expectedVal);
}

QTEST_MAIN(DataProtocolTest)

#include "tst_dataprotocol.moc"
