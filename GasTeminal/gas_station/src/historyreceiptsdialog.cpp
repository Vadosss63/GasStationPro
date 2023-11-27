#include "historyreceiptsdialog.h"

#include <QFile>
#include <QVBoxLayout>
#include <iostream>

#include "appsettings.h"
#include "dataprotocol.h"

HistoryReceiptsDialog::HistoryReceiptsDialog(QWidget* parent) : QWidget(parent)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(500, 500);
    createWidget();
}

void HistoryReceiptsDialog::showSum(int sum_)
{
    sum->setText(QString("Сумма %1 ").arg(sum_) + rubChar);
}

void HistoryReceiptsDialog::readReceiptsLogFile()
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

void HistoryReceiptsDialog::showDialog()
{
    readReceiptsLogFile();
    show();
}

void HistoryReceiptsDialog::createWidget()
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
