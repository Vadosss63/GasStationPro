/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "receipthistorywindow.h"

#include <QVBoxLayout>

ReceiptHistoryWindow::ReceiptHistoryWindow(QWidget* parent) : QWidget(parent)
{
    createWidget();
}

void ReceiptHistoryWindow::setInputSum(int sum)
{
    inputSum->setText(QString(sumTemplate).arg(sum));
}

void ReceiptHistoryWindow::settupReceiptList(const QStringList& receipts)
{
    receiptList->clear();

    for (const auto& receipt : receipts)
    {
        if (!receipt.isEmpty())
        {
            new QListWidgetItem(receipt, receiptList);
        }
    }
    constexpr int rowShift = 2;
    receiptList->setCurrentRow(receipts.size() - rowShift);
}

void ReceiptHistoryWindow::createWidget()
{
    constexpr int width  = 500;
    constexpr int height = 500;

    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(width, height);
    setWindowTitle("История операций");

    QVBoxLayout* mainLayout = new QVBoxLayout();
    QHBoxLayout* sumHLayout = new QHBoxLayout();

    inputSum = new QLabel();
    inputSum->setStyleSheet("*{font-size: 20px;}");

    receiptList = new QListWidget(this);

    sumHLayout->addWidget(inputSum);

    mainLayout->addLayout(sumHLayout);
    mainLayout->addWidget(receiptList);

    setLayout(mainLayout);
}
