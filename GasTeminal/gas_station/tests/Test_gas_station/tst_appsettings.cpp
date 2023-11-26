#include <QBuffer>
#include <QCoreApplication>
#include <QtTest>

#include "appsettings.h"

namespace
{
std::unique_ptr<QByteArray> buf = std::make_unique<QByteArray>();
}

std::unique_ptr<QIODevice> openFile(const QString&, QIODevice::OpenMode mode)
{
    std::unique_ptr<QIODevice> buffer = std::make_unique<QBuffer>(buf.get());

    buffer->open(mode);
    return buffer;
}

class appsettings : public QObject
{
    Q_OBJECT

public:
    appsettings() {}
    ~appsettings() {}

private slots:
    void initTestCase() { buf = std::make_unique<QByteArray>(); }
    void cleanupTestCase() {}

    void testParams();

    void testGetSettings();

    void testAddOneLogFile();

    void testAddTwoLogFiles();

private:
    void addLogToFile(const QString& log);
};

void appsettings::addLogToFile(const QString& log)
{
    AppSettings& settings      = AppSettings::instance();
    settings.getSettings().sum = 1000;
    settings.addTextToLogFile(log);

    QCOMPARE(settings.getSettings().sum, 0);
}

void appsettings::testParams()
{
    initTestCase();
    AppSettings& settings = AppSettings::instance();
    QCOMPARE(settings.getReceiptFolderName(), QString("./receiptCache/"));

    AppSettings::Settings& appSettings = settings.getSettings();
    QCOMPARE(appSettings.sum, 0);
}

void appsettings::testAddOneLogFile()
{
    initTestCase();

    QString receipt = "Дата: 21.07.2021 22:54\n"
                      "Колонка: 1\n"
                      "Бензин: АИ 92\n"
                      "Литры: 20 Л\n"
                      "Сумма: 1000.00₽ руб";

    addLogToFile(receipt);

    QStringList   logs            = AppSettings::instance().readLogFile();
    constexpr int expectedLogSize = 2;
    QCOMPARE(logs.size(), expectedLogSize);
    QVERIFY(logs.contains(receipt));
}

void appsettings::testAddTwoLogFiles()
{
    initTestCase();
    QString receipt1 = "Дата: 21.07.2021 22:54\n"
                       "Колонка: 1\n"
                       "Бензин: АИ 92\n"
                       "Литры: 20 Л\n"
                       "Сумма: 1000.00₽ руб";

    QString receipt2 = "Дата: 22.07.2021 22:54\n"
                       "Колонка: 1\n"
                       "Бензин: АИ 92\n"
                       "Литры: 20 Л\n"
                       "Сумма: 2000.00₽ руб";

    addLogToFile(receipt1);
    addLogToFile(receipt2);

    AppSettings&  settings        = AppSettings::instance();
    QStringList   logs            = settings.readLogFile();
    constexpr int expectedLogSize = 3;
    QCOMPARE(logs.size(), expectedLogSize);
    QVERIFY(logs.contains(receipt1));
    QVERIFY(logs.contains(receipt2));
}

QTEST_MAIN(appsettings)

#include "tst_appsettings.moc"
