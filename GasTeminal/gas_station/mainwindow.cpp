#include "mainwindow.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDate>
#include <QErrorMessage>
#include <QFile>
#include <QKeyEvent>
#include <QSettings>
#include <QSpacerItem>
#include <QThread>
#include <iostream>

#include <qmap.h>

#include "appsettings.h"

MainWindow::MainWindow()
{
    readSettings();
    readConfig();

    port = new Port();
    connect(port, SIGNAL(error_(QString)), this, SLOT(printLog(QString)));
    connect(port, SIGNAL(readyData()), this, SLOT(readDataFromPort()));

    /// TODO: linux
    ///  grep -i 'tty' /var/log/dmesg
    port->writeSettingsPort(QString(configure.comPort), configure.baudRate);
    port->connectPort();

    serviceMenuWindow     = new ServiceMenuWindow();
    historyReceiptsDialog = new HistoryReceiptsDialog();
    createWidget();

    ReceivedData data{};
    data.balance     = 0;
    data.isActiveBtn = 0;

    setShowData(data);
    phoneOfSupportLable->setText(configure.phoneOfSupport);
    setVisibleSecondBtn(configure.activeBtn2);
}

MainWindow::~MainWindow()
{
    delete historyReceiptsDialog;
    delete serviceMenuWindow;
    port->disconnectPort();
    delete port;
}

void MainWindow::readConfig()
{
    QString filePath =
#ifndef QT_DEBUG
        "settings.json";
#else
        "/home/vadosss63/MyProject/GasStationPro/GasTeminal/gas_station/"
        "settings/settings.json";
#endif
    Configure conf;
    if (!readConfigure(filePath, conf))
    {
        (new QErrorMessage(this))->showMessage("The settings.json contains invalid fields!");
    }
    configure = conf;
}

ReceivedData& MainWindow::getReceivedData()
{
    return receiveData;
}

ResponseData& MainWindow::getResponseData()
{
    return sendData;
}

void MainWindow::showServiceMenu()
{
    serviceMenuWindow->setAzsNodes(currentAzsNodes);
    serviceMenuWindow->show();
    serviceMenuWindow->setFocus();
    getCounters();
    serviceMenuWindow->setFocus();
}

void MainWindow::startFirstAzsNode()
{
    clickedFirstHWBtn();
    getResponseData().state = ResponseData::isPressedFirstBtn;
}

void MainWindow::startSecondAzsNode()
{
    clickedSecondHWBtn();
    getResponseData().state = ResponseData::isPressedSecondBtn;
}

void MainWindow::clickedFirstHWBtn()
{
    constexpr int idexBtn = 1;
    saveReceipt(idexBtn);
}

void MainWindow::clickedSecondHWBtn()
{
    constexpr int idexBtn = 2;
    saveReceipt(idexBtn);
}

void MainWindow::saveReceipt(int numOfAzsNode)
{
    QString receipt = getReceipt(numOfAzsNode);
    AppSettings::instance().addTextToLogFile(receipt);
}

void MainWindow::sendToPort(const QString& data)
{
    port->writeToPort(data);
}

void MainWindow::sendToPort(const QByteArray& data)
{
    port->writeToPort(data);
    printLog(data);
}

void MainWindow::sendToPort(const std::string& data)
{
    sendToPort(QString::fromStdString(data));
}

void MainWindow::setupPrice()
{
    setAzsNode(serviceMenuWindow->getAzsNodes());
    writeSettings();
}

void MainWindow::setShowData(const ReceivedData& data)
{
    setBalance(data.balance);
    setEnabledStart(data);
    setCountOfLitres();
}

void MainWindow::setAzsNode(const std::array<ResponseData::AzsNode, countAzsNode>& azsNodes)
{
    currentAzsNodes = azsNodes;
    for (size_t i = 0; i < azsNodes.size(); ++i)
    {
        double price                        = azsNodes[i].price;
        getResponseData().azsNodes[i].price = static_cast<uint16_t>(price * 100);
        azsNodeWidgets[i].pricePerLitreLable->setText(azsNodeWidgets[i].gasTypeLable +
                                                      QString(" %1").arg(price, 0, 'f', 2) + rubChar + "/Л");

        getResponseData().azsNodes[i].gasType = azsNodes[i].gasType;
        azsNodeWidgets[i].gasTypeLable        = getGasTypeString(azsNodes[i].gasType);
    }
}

void MainWindow::setBalance(double price)
{
    balance = price;
    balanceLable->setText(QString("%1").arg(price, 0, 'f', 2) + rubChar);
    AppSettings::instance().getSettings().sum = price;
}

void MainWindow::setCountOfLitres()
{
    for (int i = 0; i < countAzsNode; ++i)
    {
        double count = azsNodeWidgets[i].startBtn->isEnabled() ? balance / currentAzsNodes[i].price : 0;
        azsNodeWidgets[i].countLitresLable->setText(QString("%1 Л").arg(count, 0, 'f', 2));
    }
}

void MainWindow::setVisibleSecondBtn(bool isVisible)
{
    constexpr int index = 1;
    azsNodeWidgets[index].pricePerLitreLable->setVisible(isVisible);
    azsNodeWidgets[index].startBtn->setVisible(isVisible);
    azsNodeWidgets[index].countLitresLable->setVisible(isVisible);
}

void MainWindow::closeServiceMenu()
{
    serviceMenuWindow->close();
}

void MainWindow::updateStateOfBtn()
{
    switch (getReceivedData().isClickedBtn)
    {
        case 0xFF:
            showServiceMenu();
            break;
        case 0xFE:
            closeServiceMenu();
            break;
        case 0x01:
            clickedFirstHWBtn();
            break;
        case 0x02:
            clickedSecondHWBtn();
            break;
        default:
            break;
    }
}

void MainWindow::readDataFromPort()
{
    QByteArray data = port->getData();
    printLog(data);

    ReceivedData* tmp = ReceivedData::getReceivedData(data);
    if (!tmp)
    {
        printLog(QString("ReceivedData is invalid"));
        return;
    }

    getReceivedData() = *tmp;
    updateStateOfBtn();

    setShowData(getReceivedData());
    sendToPort(getResponseData().getQByteArray());
    getResponseData().state = ResponseData::defaultVal;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_1:
            showServiceMenu();
            break;
        case Qt::Key_2:
            resetCounters();
            break;
        case Qt::Key_3:
            historyReceiptsDialog->showDialog();
            break;
        case Qt::Key_Escape:
            qApp->exit(0);
            break;
        default:
            break;
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::printLog(const QString& log)
{
    std::cout << log.toStdString() << std::endl;
}

void MainWindow::printLog(const QByteArray& data)
{
    QString print;
    foreach(auto b, data)
    {
        print.append("0x" + QString::number((uint8_t)b, 16) + " ");
    }
    printLog(print);
}

void MainWindow::getCounters()
{
    serviceMenuWindow->setupInfo(getReceivedData());
}

void MainWindow::resetCounters()
{
    getResponseData().state = ResponseData::resetCounters;
}

void MainWindow::createWidget()
{
    for (int i = 0; i < countAzsNode; ++i)
    {
        azsNodeWidgets[i].pricePerLitreLable = new Label(this);
        azsNodeWidgets[i].countLitresLable   = new Label(this);
        azsNodeWidgets[i].pricePerLitreLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        azsNodeWidgets[i].countLitresLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");

        azsNodeWidgets[i].startBtn = new QPushButton(this);

        connect(azsNodeWidgets[i].startBtn, SIGNAL(clicked()), this, SLOT(startFirstAzsNode()));
    }
    int indexWidget = 0;
    azsNodeWidgets[indexWidget].pricePerLitreLable->setGeometry(16, 664, 335, 36);
    azsNodeWidgets[indexWidget].pricePerLitreLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].countLitresLable->setGeometry(16, 664 + 36, 335, 36);
    azsNodeWidgets[indexWidget].countLitresLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].startBtn->setGeometry(16, 720, 335, 299);
    azsNodeWidgets[indexWidget].startBtn->setStyleSheet(
        "QPushButton{background: #00000000 url(:/images/image/btn1.png);}"
        "QPushButton:disabled{"
        "background: #00000000 url(:/images/image/btn1_disable.png);"
        "}");

    indexWidget = 1;
    azsNodeWidgets[indexWidget].pricePerLitreLable->setGeometry(918, 664, 335, 36);
    azsNodeWidgets[indexWidget].pricePerLitreLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].countLitresLable->setGeometry(918, 664 + 36, 335, 36);
    azsNodeWidgets[indexWidget].countLitresLable->setAlignment(Qt::AlignCenter);
    azsNodeWidgets[indexWidget].startBtn->setGeometry(918, 720, 335, 299);
    azsNodeWidgets[indexWidget].startBtn->setStyleSheet(
        "QPushButton{background: #00000000 url(:/images/image/btn2.png);}"
        "QPushButton:disabled{"
        "background: #00000000 url(:/images/image/btn2_disable.png);"
        "}");

    balanceLable = new Label(this);
    balanceLable->setStyleSheet("color: #003EC9; font: 210px 'Arial Black';");
    balanceLable->setGeometry(47, 315, 1178, 270);
    balanceLable->setAlignment(Qt::AlignCenter);

    phoneOfSupportLable = new Label(this);
    phoneOfSupportLable->setGeometry(386, 932, 506, 78);
    phoneOfSupportLable->setStyleSheet("color: #003EC9; font: 34px 'Arial Black';");
    phoneOfSupportLable->setAlignment(Qt::AlignCenter);

    setAzsNode(currentAzsNodes);

    setBalance(0);
    setCountOfLitres();

    connect(serviceMenuWindow, SIGNAL(setPrice()), this, SLOT(setupPrice()));
    connect(serviceMenuWindow, SIGNAL(getCounters()), this, SLOT(getCounters()));
    connect(serviceMenuWindow, SIGNAL(resetCounters()), this, SLOT(resetCounters()));
    connect(serviceMenuWindow, SIGNAL(showStatistics()), historyReceiptsDialog, SLOT(showDialog()));
    QPixmap  bkgnd(":/images/image/background.png");
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
}

void MainWindow::writeSettings()
{
    QSettings settings("Gas Station");
    settings.beginGroup("parms");
    for (int i = 0; i < countAzsNode; ++i)
    {
        settings.setValue("currentPrice" + QString::number(i), currentAzsNodes[i].price);
        settings.setValue("gasTypes" + QString::number(i), currentAzsNodes[i].gasType);
    }
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("Gas Station");
    settings.beginGroup("parms");
    bool ok = false;
    for (int i = 0; i < countAzsNode; ++i)
    {
        currentAzsNodes[i].price = settings.value("currentPrice" + QString::number(i)).toFloat();
        currentAzsNodes[i].gasType =
            static_cast<ResponseData::GasType>(settings.value("gasTypes" + QString::number(i)).toUInt(&ok));
    }
    settings.endGroup();
}

void MainWindow::setEnabledStart(const ReceivedData& showData)
{
    for (int i = 0; i < countAzsNode; ++i)
    {
        azsNodeWidgets[i].startBtn->setEnabled(showData.getIsActiveBtn(i));
    }
}

QString MainWindow::getReceipt(int numOfAzsNode)
{
    int   azsNodeIndex = numOfAzsNode - 1;
    QDate currentDate  = QDate::currentDate(); // возвращаем текущую дату
    QTime currentTime  = QTime::currentTime(); // возвращаем текущее время

    QString date = currentDate.toString("dd.MM.yyyy");
    QString time = currentTime.toString("hh:mm");
    QString text = "Дата: ";
    text.append(date);
    text.append(' ');
    text.append(time);
    text.append('\n');
    text.append("Колонка: ");
    text.append(QString::number(numOfAzsNode));
    text.append('\n');
    text.append("Бензин: ");
    text.append(azsNodeWidgets[azsNodeIndex].gasTypeLable);
    text.append('\n');
    text.append("Литры: ");
    text.append(azsNodeWidgets[azsNodeIndex].countLitresLable->text());
    text.append('\n');
    text.append("Сумма: ");
    text.append(balanceLable->text() + " руб");
    return text;
}
