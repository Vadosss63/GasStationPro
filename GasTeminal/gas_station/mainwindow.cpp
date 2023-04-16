#include "mainwindow.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QErrorMessage>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QSettings>
#include <QSpacerItem>
#include <QString>
#include <QThread>
#include <iostream>

#include <qmap.h>

#include "appsettings.h"
#include "httprequest.h"

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
    data.balanceCash     = 100;
    data.balanceCashless = 1000;
    data.isActiveBtn     = 2;

    setShowData(data);
    phoneOfSupportLable->setText(configure.phoneOfSupport);
    setVisibleSecondBtn(configure.activeBtn2);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendToServer()));
    timer->start(10000);
}

MainWindow::~MainWindow()
{
    timer->stop();
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
        "/home/makarov/projects/GasStationPro/GasTeminal/gas_station/"
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
    getCounters();
    serviceMenuWindow->show();
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
    setBalance(data.balanceCash, data.balanceCashless);
    setEnabledStart(data);
    setCountOfLitres();
}

void MainWindow::sendReport()
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("name", configure.name);
    params.addQueryItem("address", configure.address);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("count_colum", QString("%1").arg((int)(countAzsNode)));

    ReceivedData receivedData = getReceivedData();
    QStringList  typeFuel;
    for (const auto& azsNodeWidget : azsNodeWidgets)
    {
        typeFuel.append(azsNodeWidget.pricePerLitreLableCash->text());

        if (configure.showSecondPrice)
        {
            typeFuel.append(azsNodeWidget.pricePerLitreLableCashless->text());
        }
    }

    params.addQueryItem("stats", receivedData.getJsonReport(countAzsNode, typeFuel));
    std::cout << sendPost(configure.host + "/azs_stats", params).msg.toStdString() << std::endl;
}

bool MainWindow::sendReceipt(const Receipt& receipt) const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("receipt", receipt.getReceiptJson());
    Answer answer = sendPost(configure.host + "/azs_receipt", params);
    std::cout << answer.msg.toStdString() << std::endl;

    return answer.isOk;
}

void MainWindow::saveReceipt(int numOfAzsNode)
{
    sendReceiptFiles();

    Receipt receipt = getReceipt(numOfAzsNode);

    AppSettings::instance().addTextToLogFile(receipt.getReceipt());

    if (!sendReceipt(receipt))
    {
        writeReceiptToFile(receipt);
    }
}

void MainWindow::sendReceiptFiles()
{
    QString     folderName      = AppSettings::instance().getReceiptFolderName();
    QStringList listReciptFiles = getListReciptFiles();

    for (const QString& fileName : listReciptFiles)
    {
        QFile fileReceipt(folderName + fileName);
        if (sendReciptFromFile(fileReceipt))
        {
            fileReceipt.remove();
        }
        else
        {
            return;
        }
    }
}

bool MainWindow::sendReciptFromFile(QFile& fileReceipt) const
{
    Receipt receipt{};

    if (readReceiptFromFile(fileReceipt, receipt))
    {
        return sendReceipt(receipt);
    }

    fileReceipt.remove();

    return false;
}

AzsButton MainWindow::getServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    Answer    answer = sendPost(configure.host + "/get_azs_button", params);
    AzsButton azsButton;
    if (!answer.isOk)
    {
        std::cerr << answer.msg.toStdString() << std::endl;
    }
    azsButton.readAzsButton(answer.msg);

    return azsButton;
}

bool MainWindow::resetServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);

    Answer answer = sendPost(configure.host + "/reset_azs_button", params);
    if (!answer.isOk)
    {
        std::cerr << answer.msg.toStdString() << std::endl;
    }
    return answer.isOk;
}

void MainWindow::setAzsNode(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    currentAzsNodes = azsNodes;
    for (size_t i = 0; i < azsNodes.size(); ++i)
    {
        getResponseData().azsNodes[i].priceCash     = azsNodes[i].priceCash;
        getResponseData().azsNodes[i].priceCashless = azsNodes[i].priceCashless;
        getResponseData().azsNodes[i].gasType       = azsNodes[i].gasType;
        azsNodeWidgets[i].gasTypeLable->setText(getGasTypeString(azsNodes[i].gasType));

        double priceCash     = static_cast<double>(azsNodes[i].priceCash) / 100;
        double priceCashless = static_cast<double>(azsNodes[i].priceCashless) / 100;

        if (configure.showSecondPrice)
        {
            azsNodeWidgets[i].pricePerLitreLableCash->setText(
                QString("Налич: ") + QString("%1 ").arg(priceCash, 0, 'f', 2) + rubChar + "/Л");
            azsNodeWidgets[i].pricePerLitreLableCashless->setText(
                QString("Безнал: ") + QString("%1 ").arg(priceCashless, 0, 'f', 2) + rubChar + "/Л");
        }
        else
        {
            azsNodeWidgets[i].pricePerLitreLableCash->setText(QString("%1 ").arg(priceCash, 0, 'f', 2) + rubChar +
                                                              "/Л");
        }
    }
}

void MainWindow::setBalance(double inputBalanceCash, double inputBalanceCashless)
{
    balanceCash     = inputBalanceCash;
    balanceCashless = inputBalanceCashless;
    double price    = inputBalanceCash + inputBalanceCashless;
    balanceLable->setText(QString("%1").arg(price, 0, 'f', 2) + rubChar);
    AppSettings::instance().getSettings().sum = price;
}

void MainWindow::setCountOfLitres()
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        double priceCash     = static_cast<double>(currentAzsNodes[i].priceCash) / 100.f;
        double priceCashless = static_cast<double>(currentAzsNodes[i].priceCashless) / 100.f;

        double countFuelCash     = azsNodeWidgets[i].startBtn->isEnabled() ? balanceCash / priceCash : 0;
        double countFuelCashless = azsNodeWidgets[i].startBtn->isEnabled() ? balanceCashless / priceCashless : 0;

        double countFuel = countFuelCash + countFuelCashless;
        azsNodeWidgets[i].countLitresLable->setText(QString("%1 Л").arg(countFuel, 0, 'f', 2));
    }
}

void MainWindow::setVisibleSecondBtn(bool isVisible)
{
    constexpr int index = 1;
    azsNodeWidgets[index].gasTypeLable->setVisible(isVisible);
    azsNodeWidgets[index].pricePerLitreLableCash->setVisible(isVisible);
    azsNodeWidgets[index].startBtn->setVisible(isVisible);
    azsNodeWidgets[index].countLitresLable->setVisible(isVisible);

    if (configure.showSecondPrice)
    {
        azsNodeWidgets[index].pricePerLitreLableCashless->setVisible(isVisible);
    }

    countAzsNode = isVisible ? 2 : 1;
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
} ///TODO: find price validation max price is 200

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

void MainWindow::setBtnFromServer(const AzsButton& azsButton)
{
    getResponseData().state = static_cast<ResponseData::State>(azsButton.button);
    if (azsButton.price1)
    {
        currentAzsNodes[0].priceCash = azsButton.price1; /// TODO: Add more case for priceCashless
    }
    if (azsButton.price2)
    {
        currentAzsNodes[1].priceCash = azsButton.price2;
    }
    if (azsButton.price1 || azsButton.price2) /// TODO: Have to be refactored need to modify server part
    {
        setAzsNode(currentAzsNodes);
        writeSettings();
    }
}

void MainWindow::sendToServer()
{
    const AzsButton azsButton = getServerBtn();
    if (azsButton.idAzs && (azsButton.price1 || azsButton.price2 || azsButton.button))
    {
        setBtnFromServer(azsButton);
        resetServerBtn();
    }
    sendReport();
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
        print.append("0x" + QString::number(static_cast<uint8_t>(b), 16) + " ");
    }
    printLog(print);
}

void MainWindow::getCounters()
{
    serviceMenuWindow->setupInfo(getReceivedData(), countAzsNode);
}

void MainWindow::resetCounters()
{
    getResponseData().state = ResponseData::resetCounters;
}

void MainWindow::createWidget()
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        azsNodeWidgets[i].gasTypeLable           = new Label(this);
        azsNodeWidgets[i].pricePerLitreLableCash = new Label(this);
        azsNodeWidgets[i].countLitresLable       = new Label(this);
        azsNodeWidgets[i].gasTypeLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        azsNodeWidgets[i].pricePerLitreLableCash->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        azsNodeWidgets[i].countLitresLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");

        if (configure.showSecondPrice)
        {
            azsNodeWidgets[i].pricePerLitreLableCashless = new Label(this);
            azsNodeWidgets[i].pricePerLitreLableCashless->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        }

        azsNodeWidgets[i].startBtn = new QPushButton(this);
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

    if (configure.showSecondPrice)
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

    balanceLable = new Label(this);
    balanceLable->setStyleSheet("color: #003EC9; font: 210px 'Arial Black';");
    balanceLable->setGeometry(47, 315, 1178, 270);
    balanceLable->setAlignment(Qt::AlignCenter);

    phoneOfSupportLable = new Label(this);
    phoneOfSupportLable->setGeometry(386, 932, 506, 78);
    phoneOfSupportLable->setStyleSheet("color: #003EC9; font: 34px 'Arial Black';");
    phoneOfSupportLable->setAlignment(Qt::AlignCenter);

    setAzsNode(currentAzsNodes);

    setBalance(0, 0);
    setCountOfLitres();

    connect(serviceMenuWindow, SIGNAL(setPrice()), this, SLOT(setupPrice()));
    connect(serviceMenuWindow, SIGNAL(getCounters()), this, SLOT(getCounters()));
    connect(serviceMenuWindow, SIGNAL(resetCounters()), this, SLOT(resetCounters()));
    connect(serviceMenuWindow, SIGNAL(showStatistics()), historyReceiptsDialog, SLOT(showDialog()));
    connect(azsNodeWidgets[0].startBtn, SIGNAL(clicked()), this, SLOT(startFirstAzsNode()));
    connect(azsNodeWidgets[1].startBtn, SIGNAL(clicked()), this, SLOT(startSecondAzsNode()));
    QPixmap  bkgnd(":/images/image/background.png");
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
}

void MainWindow::writeSettings()
{
    QSettings settings("Gas Station");
    settings.beginGroup("parms");
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        settings.setValue("currentPriceCash" + QString::number(i), currentAzsNodes[i].priceCash);
        settings.setValue("currentPriceCashless" + QString::number(i), currentAzsNodes[i].priceCashless);
        settings.setValue("gasTypes" + QString::number(i), currentAzsNodes[i].gasType);
    }
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("Gas Station");
    settings.beginGroup("parms");
    bool ok = false;
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        currentAzsNodes[i].priceCash     = settings.value("currentPriceCash" + QString::number(i)).toInt();
        currentAzsNodes[i].priceCashless = settings.value("currentPriceCashless" + QString::number(i)).toInt();
        currentAzsNodes[i].gasType =
            static_cast<ResponseData::GasType>(settings.value("gasTypes" + QString::number(i)).toUInt(&ok));
    }
    settings.endGroup();
}

void MainWindow::setEnabledStart(const ReceivedData& showData)
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        azsNodeWidgets[i].startBtn->setEnabled(showData.getIsActiveBtn(i));
    }
}

Receipt MainWindow::getReceipt(int numOfAzsNode)
{
    const int azsNodeIndex = numOfAzsNode - 1;

    Receipt receipt{};

    receipt.time         = static_cast<int>(QDateTime::currentSecsSinceEpoch());
    receipt.date         = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    receipt.numOfAzsNode = numOfAzsNode;
    receipt.gasType      = azsNodeWidgets[azsNodeIndex].gasTypeLable->text();
    receipt.countLitres  = azsNodeWidgets[azsNodeIndex].countLitresLable->text();
    receipt.sum          = balanceLable->text().replace("Р", "");
    return receipt;
}
