#include "historyreceiptswindow.h"

#include <QFile>
#include <QVBoxLayout>
#include <iostream>

#include "appsettings.h"
#include "dataprotocol.h"

ReceiptHistoryWindow::ReceiptHistoryWindow(QWidget* parent) : QWidget(parent)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(500, 500);
    createWidget();
}

void ReceiptHistoryWindow::showSum(int sum_)
{
    sum->setText(QString("Сумма %1 ").arg(sum_) + rubChar);
}

void ReceiptHistoryWindow::readReceiptsLogFile()
{
    auto receipts = AppSettings::instance().readLogFile();
    showSum(AppSettings::instance().getSettings().sum);
    receiptList->clear();

    for (const auto& receipt : receipts)
    {
        if (!receipt.isEmpty())
        {
            new QListWidgetItem(receipt, receiptList);
        }
    }
    receiptList->setCurrentRow(receipts.size() - 2);
}

void ReceiptHistoryWindow::showDialog()
{
    readReceiptsLogFile();
    show();
}

void ReceiptHistoryWindow::createWidget()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* sumHLayout = new QHBoxLayout();
    sum                     = new QLabel(QString("Сумма = 0 ") + rubChar);

    sum->setStyleSheet("*{font-size: 20px;}");
    sumHLayout->addWidget(sum);
    mainLayout->addLayout(sumHLayout);
    receiptList = new QListWidget(this);
    mainLayout->addWidget(receiptList);
    setLayout(mainLayout);
    setWindowTitle("История операций");
}
