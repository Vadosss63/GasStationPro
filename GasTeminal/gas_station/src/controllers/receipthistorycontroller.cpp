#include "receipthistorycontroller.h"

#include "appsettings.h"

ReceiptHistoryController::ReceiptHistoryController(QObject* parent) : QObject(parent)
{
    historyReceiptsDialog = std::make_unique<ReceiptHistoryWindow>();
}

void ReceiptHistoryController::showDialog()
{
    const QStringList receipts = AppSettings::instance().readLogFile();
    const int         sum      = AppSettings::instance().getSettings().sum;

    historyReceiptsDialog->setInputSum(sum);
    historyReceiptsDialog->settupReceiptList(receipts);

    historyReceiptsDialog->show();
}
