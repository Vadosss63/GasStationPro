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
constexpr auto dateToken         = "date";
constexpr auto numOfAzsNodeToken = "num_azs_node";
constexpr auto gasTypeToken      = "gas_type";
constexpr auto countLitresToken  = "count_litres";
constexpr auto cashToken         = "cash";
constexpr auto cashlessToken     = "cashless";
constexpr auto onlineToken       = "online";
constexpr auto sumToken          = "sum";

constexpr auto requiredFields = {timeToken,
                                 dateToken,
                                 numOfAzsNodeToken,
                                 gasTypeToken,
                                 countLitresToken,
                                 cashToken,
                                 cashlessToken,
                                 onlineToken,
                                 sumToken};
}

QString Receipt::getReceipt() const
{
    return QString("Дата: %1\n"
                   "Колонка: %2\n"
                   "Топливо: %3\n"
                   "Литры: %4\n"
                   "Безнал: %5\n"
                   "Налич: %6\n"
                   "Онлайн: %7\n"
                   "Сумма: %8 руб")
        .arg(date,
             QString::number(numOfAzsNode),
             gasType,
             QString::number(countLitres, 'f', 2),
             QString::number(cashless, 'f', 2),
             QString::number(cash, 'f', 2),
             QString::number(online, 'f', 2),
             QString::number(sum, 'f', 2));
}

QString Receipt::getReceiptJson() const
{
    const QJsonObject receipt{{timeToken, time},
                              {dateToken, date},
                              {numOfAzsNodeToken, numOfAzsNode},
                              {gasTypeToken, gasType},
                              {countLitresToken, countLitres},
                              {cashToken, cash},
                              {cashlessToken, cashless},
                              {onlineToken, online},
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
    if (!fileReceipt)
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
    if (!fileReceipt)
    {
        LOG_ERROR("Can not open json file");
        return std::nullopt;
    }

    const QString jsonData = fileReceipt->readAll();
    fileReceipt->close();

    const QJsonDocument doc  = QJsonDocument::fromJson(jsonData.toUtf8());
    const QJsonObject   json = doc.object();

    for (const auto& field : requiredFields)
    {
        if (!json.contains(field))
        {
            LOG_ERROR("Missing or invalid field in settings: " + QString(field));
            return std::nullopt;
        }
    }

    Receipt receipt{.time         = json[timeToken].toInt(),
                    .date         = json[dateToken].toString(),
                    .numOfAzsNode = json[numOfAzsNodeToken].toInt(),
                    .gasType      = json[gasTypeToken].toString(),
                    .countLitres  = json[countLitresToken].toDouble(),
                    .cash         = json[cashToken].toDouble(),
                    .cashless     = json[cashlessToken].toDouble(),
                    .online       = json[onlineToken].toDouble(),
                    .sum          = json[sumToken].toDouble()

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
