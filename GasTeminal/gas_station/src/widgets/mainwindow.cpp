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

#include "mainwindow.h"

#include <QKeyEvent>

#include "azsnodesettings.h"

MainWindow::MainWindow()
{
    isVisibleBtns = readVisibleBtns();
}

MainWindow::~MainWindow() {}

void MainWindow::setSupportPhone(const QString& phone)
{
    supportPhoneLabel->setText(phone);
}

void MainWindow::setVisibleSecondBtn(bool isVisible, bool showSecondPrice)
{
    constexpr int index = 1;
    azsNodeWidgets[index].gasTypeLable->setVisible(isVisible);
    azsNodeWidgets[index].pricePerLitreLableCash->setVisible(isVisible);
    azsNodeWidgets[index].startBtn->setVisible(isVisible);
    azsNodeWidgets[index].countLitresLable->setVisible(isVisible);

    if (showSecondPrice)
    {
        azsNodeWidgets[index].pricePerLitreLableCashless->setVisible(isVisible);
    }
    updateVisibleBtns();
}

void MainWindow::setBalance(double balance)
{
    balanceLabel->setText(QString("%1Р").arg(balance, 0, 'f', 2));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_0)
    {
        isVisibleBtns = !isVisibleBtns;
        writeVisibleBtns(isVisibleBtns);
        updateVisibleBtns();
        return;
    }

    if (iKeyPressEvent)
    {
        iKeyPressEvent->keyPressEvent(event);
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::updateVisibleBtns()
{
    constexpr int btn1Idx = 0;

    constexpr int btn2Idx = 1;

    azsNodeWidgets[btn1Idx].startBtn->setVisible(isVisibleBtns);

    bool isVisibleBtn2 = azsNodeWidgets[btn2Idx].gasTypeLable->isVisible() && isVisibleBtns;

    azsNodeWidgets[btn2Idx].startBtn->setVisible(isVisibleBtn2);
}

void MainWindow::disableAzs(bool disable)
{
    temporarilyUnavailableWidget->setFocusPolicy(Qt::StrongFocus);
    if (disable)
    {
        temporarilyUnavailableWidget->showFullScreen();
        temporarilyUnavailableWidget->setFocus();
    }
    else
    {
        temporarilyUnavailableWidget->setVisible(false);
    }
}

bool MainWindow::isBlock()
{
    return temporarilyUnavailableWidget->isVisible();
}

void MainWindow::createWidget(bool showSecondPrice)
{
    for (int nodeId = 0; nodeId < maxAzsNodeCount; ++nodeId)
    {
        azsNodeWidgets[nodeId].gasTypeLable           = new LabelWidget(this);
        azsNodeWidgets[nodeId].pricePerLitreLableCash = new LabelWidget(this);
        azsNodeWidgets[nodeId].countLitresLable       = new LabelWidget(this);
        azsNodeWidgets[nodeId].gasTypeLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        azsNodeWidgets[nodeId].pricePerLitreLableCash->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        azsNodeWidgets[nodeId].countLitresLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");

        if (showSecondPrice)
        {
            azsNodeWidgets[nodeId].pricePerLitreLableCashless = new LabelWidget(this);
            azsNodeWidgets[nodeId].pricePerLitreLableCashless->setStyleSheet(
                "color: #003EC9; font: 30px 'Arial Black';");
        }

        azsNodeWidgets[nodeId].startBtn = new QPushButton(this);
    }

    int indexWidget = 0;
    azsNodeWidgets[indexWidget].gasTypeLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].pricePerLitreLableCash->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].countLitresLable->setGeometry(16, 664 + 36, 335, 36);
    azsNodeWidgets[indexWidget].countLitresLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].startBtn->setGeometry(16, 720, 335, 299);
    azsNodeWidgets[indexWidget].startBtn->setStyleSheet(
        "QPushButton{background: #00000000 url(:/images/image/btn1.png);}"
        "QPushButton:disabled{"
        "background: #00000000 url(:/images/image/btn1_disable.png);"
        "}");

    indexWidget = 1;
    azsNodeWidgets[indexWidget].gasTypeLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].pricePerLitreLableCash->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].countLitresLable->setGeometry(918, 664 + 36, 335, 36);
    azsNodeWidgets[indexWidget].countLitresLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].startBtn->setGeometry(918, 720, 335, 299);
    azsNodeWidgets[indexWidget].startBtn->setStyleSheet(
        "QPushButton{background: #00000000 url(:/images/image/btn2.png);}"
        "QPushButton:disabled{"
        "background: #00000000 url(:/images/image/btn2_disable.png);"
        "}");

    if (showSecondPrice)
    {
        indexWidget = 0;
        azsNodeWidgets[indexWidget].gasTypeLable->setGeometry(16, 604, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCash->setGeometry(16, 634, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCashless->setGeometry(16, 664, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCashless->setAlignment(Qt::AlignCenter);

        indexWidget = 1;
        azsNodeWidgets[indexWidget].gasTypeLable->setGeometry(918, 604, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCash->setGeometry(918, 634, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCashless->setGeometry(918, 664, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCashless->setAlignment(Qt::AlignCenter);
    }
    else
    {
        indexWidget = 0;
        azsNodeWidgets[indexWidget].gasTypeLable->setGeometry(16, 634, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCash->setGeometry(16, 664, 335, 36);

        indexWidget = 1;
        azsNodeWidgets[indexWidget].gasTypeLable->setGeometry(918, 634, 335, 36);
        azsNodeWidgets[indexWidget].pricePerLitreLableCash->setGeometry(918, 664, 335, 36);
    }

    balanceLabel = new LabelWidget(this);
    balanceLabel->setStyleSheet("color: #003EC9; font: 210px 'Arial Black';");
    balanceLabel->setGeometry(47, 315, 1178, 270);
    balanceLabel->setAlignment(Qt::AlignCenter);

    supportPhoneLabel = new LabelWidget(this);
    supportPhoneLabel->setGeometry(386, 932, 506, 78);
    supportPhoneLabel->setStyleSheet("color: #003EC9; font: 34px 'Arial Black';");
    supportPhoneLabel->setAlignment(Qt::AlignCenter);

    QPixmap  bkgnd(":/images/image/background.png");
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    temporarilyUnavailableWidget = new TemporarilyUnavailableWidget("Терминал временно не работает", this);
    temporarilyUnavailableWidget->hide();

    connect(azsNodeWidgets[0].startBtn, SIGNAL(clicked()), this, SIGNAL(startFirstAzsNode()));
    connect(azsNodeWidgets[1].startBtn, SIGNAL(clicked()), this, SIGNAL(startSecondAzsNode()));
    updateVisibleBtns();
}

void MainWindow::setKeyPressEvent(IKeyPressEvent* newIKeyPressEvent)
{
    iKeyPressEvent = newIKeyPressEvent;
}

void MainWindow::setEnabledStartBtn(bool isEnabled, size_t nodeId)
{
    azsNodeWidgets[nodeId].startBtn->setEnabled(isEnabled);
}

QString MainWindow::getCountLitresStr(size_t nodeId) const
{
    return azsNodeWidgets[nodeId].countLitresLable->text();
}

QString MainWindow::getGasTypeStr(size_t nodeId) const
{
    return azsNodeWidgets[nodeId].gasTypeLable->text();
}

void MainWindow::setGasType(QString gasType, size_t nodeId)
{
    azsNodeWidgets[nodeId].gasTypeLable->setText(gasType);
}

void MainWindow::setPricePerLitreLableCash(double priceCash, size_t nodeId)
{
    azsNodeWidgets[nodeId].pricePerLitreLableCash->setText(QString("Налич: %1 Р/Л").arg(priceCash, 0, 'f', 2));
}

void MainWindow::setPricePerLitreLableCashless(double price, size_t nodeId)
{
    azsNodeWidgets[nodeId].pricePerLitreLableCashless->setText(QString("Безнал: %1 Р/Л").arg(price, 0, 'f', 2));
}

void MainWindow::setPricePerLitreLable(double priceCash, size_t nodeId)
{
    azsNodeWidgets[nodeId].pricePerLitreLableCash->setText(QString("%1 Р/Л").arg(priceCash, 0, 'f', 2));
}

void MainWindow::setCountLitres(double countFuel, size_t nodeId)
{
    azsNodeWidgets[nodeId].countLitresLable->setText(QString("%1 Л").arg(countFuel, 0, 'f', 2));
}

bool MainWindow::isStartBtnEnabled(size_t nodeId)
{
    return azsNodeWidgets[nodeId].startBtn->isEnabled();
}
