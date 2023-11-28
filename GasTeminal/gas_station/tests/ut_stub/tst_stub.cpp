#include <QtTest>

class StubTest : public QObject
{
    Q_OBJECT
public:
private slots:
//    void initTestCase() {}
//    void cleanupTestCase() {}

    void testStub();
};

void StubTest::testStub()
{
    bool isOk = true;
    QVERIFY(isOk);
    int expectedVal = 2;
    int val         = 2;
    QCOMPARE(val, expectedVal);
}

QTEST_MAIN(StubTest)

#include "tst_stub.moc"
