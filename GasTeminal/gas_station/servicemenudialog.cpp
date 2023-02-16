#include "servicemenudialog.h"

#include <QFile>
#include <QVBoxLayout>
#include <iostream>

#include "dataprotocol.h"
#include "settings.h"

ServiceMenuDialog::ServiceMenuDialog(QWidget* parent) : QWidget(parent)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(500, 500);
    createWidget();
}

void ServiceMenuDialog::showSum(int sum_)
{
    sum->setText(QString("Сумма %1 ").arg(sum_) + pubChar);
}
void ServiceMenuDialog::readLogFile()
{
    auto cheques = Settings::instance().readLogFile();
    showSum(Settings::instance().getSettingsPrametrs().sum);
    chequeList->clear();

    for (auto& cheque : cheques)
    {
        if (!cheque.isEmpty()) new QListWidgetItem(cheque, chequeList);
    }
    chequeList->setCurrentRow(cheques.size() - 2);
}

void ServiceMenuDialog::showDialog()
{
    readLogFile();
    show();
}

void ServiceMenuDialog::createWidget()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* sumHLayout = new QHBoxLayout();
    sum                     = new QLabel(QString("Сумма = 0 ") + pubChar);

    sum->setStyleSheet("*{font-size: 20px;}");
    sumHLayout->addWidget(sum);
    mainLayout->addLayout(sumHLayout);
    chequeList = new QListWidget(this);
    mainLayout->addWidget(chequeList);
    setLayout(mainLayout);
}
