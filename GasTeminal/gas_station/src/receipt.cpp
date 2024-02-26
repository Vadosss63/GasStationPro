#include "receipt.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "appsettings.h"
#include "filesystemutilities.h"
#include "logging.h"

namespace
{
constexpr auto timeToken         = "time";
constexpr auto dataToken         = "data";
constexpr auto numOfAzsNodeToken = "num_azs_node";
constexpr auto gasTypeToken      = "gas_type";
constexpr auto countLitresToken  = "count_litres";
constexpr auto cashToken         = "cash";
constexpr auto cashlessToken     = "cashless";
constexpr auto sumToken          = "sum";
}

QString Receipt::getReceipt() const
{
    return QString("Дата: %1\n"
                   "Колонка: %2\n"
                   "Топливо: %3\n"
                   "Литры: %4\n"
                   "Безнал: %5\n"
                   "Налич: %6\n"
                   "Сумма: %7 руб")
        .arg(date,
             QString::number(numOfAzsNode),
             gasType,
             countLitres,
             QString::number(cashless, 'f', 2),
             QString::number(cash, 'f', 2),
             sum);
}

QString Receipt::getReceiptJson() const
{
    const QJsonObject receipt{{timeToken, time},
                              {dataToken, date},
                              {numOfAzsNodeToken, numOfAzsNode},
                              {gasTypeToken, gasType},
                              {countLitresToken, countLitres},
                              {cashToken, cash},
                              {cashlessToken, cashless},
                              {sumToken, sum}};

    QString receiptJson = QString::fromUtf8(QJsonDocument(receipt).toJson());

    return receiptJson;
}

void writeReceiptToFile(const Receipt& receipt)
{
    const QString folderPath = AppSettings::instance().getReceiptFolderName();
    const QString filePath   = QString("%1%2.json").arg(folderPath).arg(receipt.time);

    if (!createDirIfNeeded(folderPath))
    {
        LOG_ERROR("Failed to create directory: " + folderPath);
        return;
    }

    std::unique_ptr<QIODevice> fileReceipt = openFile(filePath, QIODevice::WriteOnly);
    if (!fileReceipt->isOpen())
    {
        LOG_ERROR("Failed to write Receipt to json");
        return;
    }

    const std::string dataTowrite{receipt.getReceiptJson().toStdString()};

    fileReceipt->write(dataTowrite.c_str());
    fileReceipt->close();
}

std::optional<Receipt> readReceiptFromFile(const QString& fileReceiptPath)
{
    std::unique_ptr<QIODevice> fileReceipt = openFile(fileReceiptPath, QIODevice::ReadOnly | QIODevice::Text);
    if (!fileReceipt->isOpen())
    {
        LOG_ERROR("Can not open json file");
        return std::nullopt;
    }

    const QString jsonData = fileReceipt->readAll();
    fileReceipt->close();

    const QJsonDocument doc  = QJsonDocument::fromJson(jsonData.toUtf8());
    const QJsonObject   json = doc.object();

    const QList<QString> requiredFields = {
        timeToken, dataToken, numOfAzsNodeToken, gasTypeToken, countLitresToken, cashToken, cashlessToken, sumToken};

    for (const QString& field : requiredFields)
    {
        if (!json.contains(field))
        {
            LOG_ERROR("Missing or invalid field in settings: " + field);
            return std::nullopt;
        }
    }

    Receipt receipt{.time         = json[timeToken].toInt(),
                    .date         = json[dataToken].toString(),
                    .numOfAzsNode = json[numOfAzsNodeToken].toInt(),
                    .gasType      = json[gasTypeToken].toString(),
                    .countLitres  = json[countLitresToken].toString(),
                    .cash         = json[cashToken].toDouble(),
                    .cashless     = json[cashlessToken].toDouble(),
                    .sum          = json[sumToken].toString()

    };

    return receipt;
}

QStringList getListReciptFiles()
{
    const QString folderPath = AppSettings::instance().getReceiptFolderName();

    const QRegExp     expr{"^\\d{10}\\.json$"};
    const QStringList allFiles{getDirectoryFileList(folderPath)};

    return allFiles.filter(expr);
}
