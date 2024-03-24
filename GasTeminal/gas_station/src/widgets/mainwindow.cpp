#include "mainwindow.h"

void MainWindow::setSupportPhone(const QString& phone)
{
    phoneOfSupportLable->setText(phone);
}

MainWindow::MainWindow() {}

MainWindow::~MainWindow() {}

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
}

void MainWindow::setBalance(double balance)
{
    balanceLable->setText(QString("%1Р").arg(balance, 0, 'f', 2));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (iKeyPressEvent)
    {
        iKeyPressEvent->keyPressEvent(event);
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::disableAzs(bool disable)
{
    //setDisabled(disable);
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

void MainWindow::createWidget(bool showSecondPrice)
{
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
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

    balanceLable = new LabelWidget(this);
    balanceLable->setStyleSheet("color: #003EC9; font: 210px 'Arial Black';");
    balanceLable->setGeometry(47, 315, 1178, 270);
    balanceLable->setAlignment(Qt::AlignCenter);

    phoneOfSupportLable = new LabelWidget(this);
    phoneOfSupportLable->setGeometry(386, 932, 506, 78);
    phoneOfSupportLable->setStyleSheet("color: #003EC9; font: 34px 'Arial Black';");
    phoneOfSupportLable->setAlignment(Qt::AlignCenter);

    QPixmap  bkgnd(":/images/image/background.png");
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    temporarilyUnavailableWidget = new TemporarilyUnavailableWidget("Терминал временно не работает", this);
    temporarilyUnavailableWidget->hide();

    connect(azsNodeWidgets[0].startBtn, SIGNAL(clicked()), this, SIGNAL(startFirstAzsNode()));
    connect(azsNodeWidgets[1].startBtn, SIGNAL(clicked()), this, SIGNAL(startSecondAzsNode()));
}

void MainWindow::setKeyPressEvent(IKeyPressEvent* newIKeyPressEvent)
{
    iKeyPressEvent = newIKeyPressEvent;
}

QString MainWindow::getSumStr() const
{
    return balanceLable->text().replace("Р", "");
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
