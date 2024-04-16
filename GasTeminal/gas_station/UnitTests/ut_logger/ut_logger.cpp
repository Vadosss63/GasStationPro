// clang-off
#include <QBuffer>
// clang-on

#include <CppUTest/TestHarness.h>

#include <CppUTestExt/MockSupport.h>

#include <memory>
#include <string>

#include "logger.h"

FileWriter::FileWriter(const QString&) {}

bool FileWriter::open(QIODevice::OpenMode)
{
    return true;
}

bool FileWriter::isOpen() const
{
    return mock("FileWriter").actualCall("isOpen").returnBoolValueOrDefault(true);
}

qint64 FileWriter::size() const
{
    return mock("FileWriter").actualCall("size").returnLongLongIntValueOrDefault(0);
}

qint64 FileWriter::writeData(const char* data, qint64 len)
{
    return mock("FileWriter")
        .actualCall("writeData")
        .withStringParameter("data", data)
        .withParameter("len", len)
        .returnLongLongIntValueOrDefault(0);
}

QString FileWriter::errorString() const
{
    return {};
}

std::unique_ptr<FileWriter> FileWriter::openFile(const QString& path, QIODevice::OpenMode mode)
{
    mock("FileWriter")
        .actualCall("openFile")
        .withStringParameter("dirPath", path.toStdString().c_str())
        .withParameter("OpenMode", mode);

    return std::unique_ptr<FileWriter>(new FileWriter(""));
}

std::unique_ptr<FileWriter> FileWriter::openLatestFileInDir(const QString& dirPath, QIODevice::OpenMode mode)
{
    mock("FileWriter")
        .actualCall("openLatestFileInDir")
        .withStringParameter("dirPath", dirPath.toStdString().c_str())
        .withParameter("OpenMode", mode);

    return std::unique_ptr<FileWriter>(new FileWriter(""));
}

bool createDirIfNeeded(const QString& dirPath)
{
    return mock("filesystemutilities")
        .actualCall("createDirIfNeeded")
        .withStringParameter("dirPath", dirPath.toStdString().c_str())
        .returnBoolValueOrDefault(true);
}

bool isDirectoryExist(const QString& dirPath)
{
    return mock("filesystemutilities")
        .actualCall("isDirectoryExist")
        .withStringParameter("dirPath", dirPath.toStdString().c_str())
        .returnBoolValueOrDefault(true);
}

int getNumberOfFilesInDir(const QString& dirPath)
{
    return mock("filesystemutilities")
        .actualCall("getNumberOfFilesInDir")
        .withStringParameter("dirPath", dirPath.toStdString().c_str())
        .returnIntValueOrDefault(0);
}

void removeOlderFilesInDir(const QString& dirPath, qint64 maxFileNumber)
{
    mock("filesystemutilities")
        .actualCall("removeOlderFilesInDir")
        .withStringParameter("dirPath", dirPath.toStdString().c_str())
        .withParameter("maxFileNumber", maxFileNumber);
}

QString getCurrentTimestamp()
{
    return mock("utilities").actualCall("getCurrentTimestamp").returnStringValue();
}

TEST_GROUP (LoggerTest)
{
protected:
    static void expectGetNumberOfFilesInDir(const std::string& expectedDirPath, int retValue)
    {
        mock("filesystemutilities")
            .expectOneCall("getNumberOfFilesInDir")
            .withStringParameter("dirPath", expectedDirPath.c_str())
            .andReturnValue(retValue);
    }

    static void expectCreateDirIfNeeded(const std::string& expectedDirPath, bool retValue = true)
    {
        mock("filesystemutilities")
            .expectOneCall("createDirIfNeeded")
            .withStringParameter("dirPath", expectedDirPath.c_str())
            .andReturnValue(retValue);
    }

    static void expectIsDirectoryExist(const std::string& expectedDirPath, bool retValue = true)
    {
        mock("filesystemutilities")
            .expectOneCall("isDirectoryExist")
            .withStringParameter("dirPath", expectedDirPath.c_str())
            .andReturnValue(retValue);
    }

    static void expectRemoveOlderFilesInDir(const std::string& expectedDirPath, qint64 expectedMaxFileNumber)
    {
        mock("filesystemutilities")
            .expectOneCall("removeOlderFilesInDir")
            .withStringParameter("dirPath", expectedDirPath.c_str())
            .withParameter("maxFileNumber", expectedMaxFileNumber);
    }

    static void expectGetCurrentTimestamp(const char* expectedTimestamp)
    {
        mock("utilities").expectOneCall("getCurrentTimestamp").andReturnValue(expectedTimestamp);
    }

    static void expectIsOpen(bool retValue = true)
    {
        mock("FileWriter").expectOneCall("isOpen").andReturnValue(retValue);
    }

    static void expectSize(qint64 retValue = 0)
    {
        mock("FileWriter").expectOneCall("size").andReturnValue(retValue);
    }

    static void expectWriteData(const QByteArray& expectedData, qint64 retValue = 0)
    {
        mock("FileWriter")
            .expectOneCall("writeData")
            .withStringParameter("data", expectedData.constData())
            .withParameter("len", expectedData.size())
            .andReturnValue(retValue);
    }

    static void expectOpenFile(const std::string& expectedPath, QIODevice::OpenMode expectedMmode)
    {
        mock("FileWriter")
            .expectOneCall("openFile")
            .withStringParameter("dirPath", expectedPath.c_str())
            .withParameter("OpenMode", expectedMmode);
    }

    static void expectOpenLatestFileInDir(const std::string& expectedPath, QIODevice::OpenMode expectedMmode)
    {
        mock("FileWriter")
            .expectOneCall("openLatestFileInDir")
            .withStringParameter("dirPath", expectedPath.c_str())
            .withParameter("OpenMode", expectedMmode);
    }
};

TEST (LoggerTest, FailedToCreateDir)
{
    const QString     logDir{"111"};
    const QString     filePref{"AZS"};
    constexpr qint64  maxLogSize{202};
    constexpr qint64  maxLogFiles{1};
    const std::string expectedLogDir{"111"};

    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, false);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};
}

TEST (LoggerTest, FilesLessThenMaxInDIr)
{
    const QString    logDir{"./dir/log/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{12};
    constexpr qint64 maxLogFiles{1};

    const std::string     expectedLogDir{"./dir/log/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./dir/log/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 0);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};
}

TEST (LoggerTest, FilesMoreThenMaxInDIr)
{
    const QString    logDir{"./logs/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{202};
    constexpr qint64 maxLogFiles{1};

    const std::string     expectedLogDir{"./logs/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./logs/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 1);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};
}

TEST (LoggerTest, WriteLog)
{
    const QString    logDir{"./dir/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{202};
    constexpr qint64 maxLogFiles{1};

    const std::string     expectedLogDir{"./dir/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./dir/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 1);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};

    const QByteArray expectedLogToWrite{"1980-12-31_13:42:11 DBG/: Hello world!\n"};
    expectSize(5);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedLogToWrite, 1);
    logger.writeLog(LogLevel::DEBUG, "Hello world!");
}

TEST (LoggerTest, WriteLogExpectChangeFile)
{
    const QString    logDir{"./dir/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{3};
    constexpr qint64 maxLogFiles{4};

    const std::string     expectedLogDir{"./dir/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./dir/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 4);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};

    constexpr const char* expectedNewTimestamp{"2222-02-11_19:42:11"};
    const std::string     expectedNewFile{"./dir/AZS_2222-02-11_19:42:11.log"};
    const QByteArray      expectedLogToWrite{"2222-02-11_19:42:11 WRN/foo: Hello world!\n"};
    expectSize(5);
    expectIsOpen();
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedNewTimestamp);
    expectOpenFile(expectedNewFile, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 4);

    expectGetCurrentTimestamp(expectedNewTimestamp);
    expectWriteData(expectedLogToWrite, 1);

    logger.writeLog(LogLevel::WARNING, "Hello world!", "foo");
}

TEST (LoggerTest, setInfoPrinDifferentLevels)
{
    const QString    logDir{"./dir/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{200};
    constexpr qint64 maxLogFiles{4};

    const std::string     expectedLogDir{"./dir/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./dir/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 4);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::INFO};
    Logger         logger{std::move(setting)};

    const QString printMsg{"Print Message"};
    logger.writeLog(LogLevel::DEBUG, printMsg);

    const QByteArray expectedInfLogToWrite{"1980-12-31_13:42:11 INF/: Print Message\n"};
    expectSize(15);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedInfLogToWrite, 1);

    logger.writeLog(LogLevel::INFO, printMsg);

    const QByteArray expectedWrnLogToWrite{"1980-12-31_13:42:11 WRN/main: Print Message\n"};
    expectSize(15);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedWrnLogToWrite, 1);

    logger.writeLog(LogLevel::WARNING, printMsg, "main");

    const QByteArray expectedErrLogToWrite{"1980-12-31_13:42:11 ERR/: Print Message\n"};
    expectSize(15);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedErrLogToWrite, 1);

    logger.writeLog(LogLevel::ERROR, printMsg);
}

TEST (LoggerTest, setWrnPrinDifferentLevels)
{
    const QString    logDir{"./dir/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{200};
    constexpr qint64 maxLogFiles{4};

    const std::string     expectedLogDir{"./dir/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./dir/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 4);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::WARNING};
    Logger         logger{std::move(setting)};

    const QString printMsg{"Print Message"};
    const QString printFunc{"test"};
    logger.writeLog(LogLevel::DEBUG, printMsg, printFunc);
    logger.writeLog(LogLevel::INFO, printMsg, printFunc);

    const QByteArray expectedWrnLogToWrite{"1980-12-31_13:42:11 WRN/: Print Message\n"};
    expectSize(15);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedWrnLogToWrite, 1);

    logger.writeLog(LogLevel::WARNING, printMsg);

    const QByteArray expectedErrLogToWrite{"1980-12-31_13:42:11 ERR/test: Print Message\n"};
    expectSize(15);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedErrLogToWrite, 1);

    logger.writeLog(LogLevel::ERROR, printMsg, printFunc);
}

TEST (LoggerTest, setErrPrinDifferentLevels)
{
    const QString    logDir{"./dir/"};
    const QString    filePref{"AZS"};
    constexpr qint64 maxLogSize{200};
    constexpr qint64 maxLogFiles{4};

    const std::string     expectedLogDir{"./dir/"};
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./dir/AZS_1980-12-31_13:42:11.log"};
    expectIsDirectoryExist(expectedLogDir, false);
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 4);

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::ERROR};
    Logger         logger{std::move(setting)};

    const QString printMsg{"Print Message"};
    logger.writeLog(LogLevel::DEBUG, printMsg);
    logger.writeLog(LogLevel::INFO, printMsg);
    logger.writeLog(LogLevel::WARNING, printMsg);

    const QByteArray expectedErrLogToWrite{"1980-12-31_13:42:11 ERR/main: Print Message\n"};
    expectSize(15);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectIsOpen();
    expectWriteData(expectedErrLogToWrite, 1);

    logger.writeLog(LogLevel::ERROR, printMsg, "main");
}

TEST (LoggerTest, OpenExistedFileOnStartup)
{
    const QString     logDir{"111"};
    const QString     filePref{"AZS"};
    constexpr qint64  maxLogSize{202};
    constexpr qint64  maxLogFiles{1};
    const std::string expectedLogDir{"111"};

    expectIsDirectoryExist(expectedLogDir, true);
    expectOpenLatestFileInDir(expectedLogDir, QIODevice::WriteOnly | QIODevice::Append);
    expectSize(35);
    expectIsOpen();

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};
}

TEST (LoggerTest, OpenExistedFileOnStartupSizeMoreThenMax)
{
    const QString     logDir{"./111/"};
    const QString     filePref{"prefix"};
    constexpr qint64  maxLogSize{202};
    constexpr qint64  maxLogFiles{2};
    const std::string expectedLogDir{"./111/"};

    expectIsDirectoryExist(expectedLogDir, true);
    expectOpenLatestFileInDir(expectedLogDir, QIODevice::WriteOnly | QIODevice::Append);
    expectSize(300);
    constexpr const char* expectedTimestamp{"1980-12-31_13:42:11"};
    const std::string     expectedFilePath{"./111/prefix_1980-12-31_13:42:11.log"};
    expectCreateDirIfNeeded(expectedLogDir, true);
    expectGetCurrentTimestamp(expectedTimestamp);
    expectOpenFile(expectedFilePath, QIODevice::WriteOnly);
    expectGetNumberOfFilesInDir(expectedLogDir, 15);
    expectRemoveOlderFilesInDir(expectedLogDir, 2);
    expectIsOpen();

    LoggerSettings setting{logDir, filePref, maxLogSize, maxLogFiles, LogLevel::DEBUG};
    Logger         logger{std::move(setting)};

    const QString    printMsg{"1234 Message"};
    const QByteArray expectedErrLogToWrite{"1980-12-31_13:42:11 ERR/main: 1234 Message\n"};
    expectSize(15);
    expectIsOpen();
    expectGetCurrentTimestamp(expectedTimestamp);
    expectWriteData(expectedErrLogToWrite, 1);

    logger.writeLog(LogLevel::ERROR, printMsg, "main");
}
