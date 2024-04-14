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

#include "servicemenuwindow.h"

#include <QGridLayout>
#include <QHeaderView>

ServiceMenuWindow::ServiceMenuWindow(QWidget* parent) : QWidget(parent, Qt::Tool)
{
    QString style = R"(
QPushButton, QSpinBox {
    color: #111;
    font-size: 35px;
    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4287ff, stop: 1 #356ccc);
}
QPushButton:pressed {
    color: #111;
    background: #fff;
}
QComboBox {
    min-height: 50px;
    color: #111;
    background-color: #356ccc;
    font-size: 35px;
    padding: 4px 8px;
}
QComboBox QAbstractItemView::item {
    min-height: 50px; /* Попробуйте направить стиль конкретно на элемент */
}
QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 35px;
}
QLabel {
    color: #ddd;
    font-size: 35px;
}
)";

    setStyleSheet(style);
    setWindowTitle("Сервисное меню");
}

void ServiceMenuWindow::setupConnects()
{
    connect(setupBtn, SIGNAL(clicked()), this, SIGNAL(pressedButton()));
    connect(countersBtn, SIGNAL(clicked()), this, SIGNAL(readCounters()));
    connect(resetCountersBtn, SIGNAL(clicked()), this, SIGNAL(resetCounters()));
    connect(statisticsBtn, SIGNAL(clicked()), this, SIGNAL(showStatistics()));
}

void ServiceMenuWindow::createNodes(AzsButtonWidget* azsButtonWidget, size_t countNodes)
{
    setupButtons();
    QGridLayout* gl         = new QGridLayout;
    QHBoxLayout* controlhbl = new QHBoxLayout;

    controlhbl->addWidget(azsButtonWidget);
    controlhbl->addWidget(setupBtn);

    gl->addLayout(controlhbl, 0, 0, Qt::AlignCenter);

    QHBoxLayout* hbl = new QHBoxLayout;
    hbl->addWidget(countersBtn);
    hbl->addWidget(resetCountersBtn);
    hbl->addWidget(statisticsBtn);
    gl->addLayout(hbl, 1, 0, Qt::AlignCenter);
    createInfoTable(countNodes);
    gl->addWidget(infoTable, 2, 0, Qt::AlignCenter);
    version = new QLabel();
    gl->addWidget(version, 3, 0, Qt::AlignCenter);

    setLayout(gl);
    setupConnects();
}

void ServiceMenuWindow::setCommonCash(uint32_t commonCash)
{
    infoTable->item(0, 0)->setData(Qt::DisplayRole, QVariant(commonCash));
}

void ServiceMenuWindow::setDailyCash(uint32_t dailyCash)
{
    infoTable->item(0, 1)->setData(Qt::DisplayRole, QVariant(dailyCash));
}

void ServiceMenuWindow::setCommonCashless(uint32_t commonCashless)
{
    infoTable->item(1, 0)->setData(Qt::DisplayRole, QVariant(commonCashless));
}

void ServiceMenuWindow::setDailyCashless(uint32_t dailyCashless)
{
    infoTable->item(1, 1)->setData(Qt::DisplayRole, QVariant(dailyCashless));
}

void ServiceMenuWindow::setCommonOnline(uint32_t commonOnline)
{
    infoTable->item(2, 0)->setData(Qt::DisplayRole, QVariant(commonOnline));
}

void ServiceMenuWindow::setDailyOnline(uint32_t dailyOnline)
{
    infoTable->item(2, 1)->setData(Qt::DisplayRole, QVariant(dailyOnline));
}

void ServiceMenuWindow::setLiters(double common, double daily, size_t nodeId, size_t countAzsNode)
{
    const size_t priceTableRows = infoTable->rowCount() - countAzsNode;

    const size_t shift = priceTableRows + nodeId;

    infoTable->item(shift, 0)->setData(Qt::DisplayRole, QString("%1").arg(common, 0, 'f', 2));
    infoTable->item(shift, 1)->setData(Qt::DisplayRole, QString("%1").arg(daily, 0, 'f', 2));
}

void ServiceMenuWindow::setVersion(const QString& ver)
{
    version->setText(QString("Версия программы: %1").arg(ver));
}

void ServiceMenuWindow::setupButtons()
{
    constexpr int width = 280;

    setupBtn = new QPushButton("Установить");
    setupBtn->setFixedWidth(width);

    countersBtn = new QPushButton("Счетчики");
    countersBtn->setFixedWidth(width);

    resetCountersBtn = new QPushButton("Инкассация");
    resetCountersBtn->setFixedWidth(width);

    statisticsBtn = new QPushButton("Статистика");
    statisticsBtn->setFixedWidth(width);
}

void ServiceMenuWindow::createInfoTable(size_t countNodes)
{
    const size_t priceTableRows = 3;
    const size_t tableColoms    = 2;

    const size_t tableRows = priceTableRows + countNodes;

    infoTable = new QTableWidget(tableRows, tableColoms);
    infoTable->setStyleSheet("color: #111; font: 25px 'Arial Black'; background-color: qlineargradient( x1: 0, y1: 0, "
                             "x2: 0, y2: 1, stop: 0 #4287ff, stop: 1 #356ccc);");

    infoTable->setFixedSize(610, 293);

    infoTable->setHorizontalHeaderItem(0, new QTableWidgetItem("общ"));
    infoTable->setHorizontalHeaderItem(1, new QTableWidgetItem("инкас"));

    infoTable->setVerticalHeaderItem(0, new QTableWidgetItem("Наличн.руб"));
    infoTable->setVerticalHeaderItem(1, new QTableWidgetItem("Безнал.руб"));
    infoTable->setVerticalHeaderItem(2, new QTableWidgetItem("Онлайн.руб"));

    for (size_t i = 0; i < tableRows; ++i)
    {
        for (size_t j = 0; j < tableColoms; ++j)
        {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            infoTable->setItem(i, j, item);
        }
    }

    QHeaderView* header = infoTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setDefaultSectionSize(200);

    header = infoTable->verticalHeader();
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setDefaultSectionSize(50);

    for (size_t i = 0; i < countNodes; ++i)
    {
        infoTable->setVerticalHeaderItem(priceTableRows + i, new QTableWidgetItem(QString("%1-Литры").arg(i + 1)));
    }
}
