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
    data.balance     = 0;
    data.isActiveBtn = 0;

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
        typeFuel.append(azsNodeWidget.pricePerLitreLable->text());
    }

    params.addQueryItem("stats", receivedData.getJsonReport(countAzsNode, typeFuel));
    std::cout << sendPost(configure.host + "/azs_stats", params).msg.toStdString() << std::endl;
}

bool MainWindow::sendReceipt(const Receipt& receipt)
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
    QString folderName = AppSettings::instance().getReceiptFolderName();
    QRegExp expr("\\d+\\.json");
    QDir    dir(folderName);

    if (dir.exists())
    {
        QStringList allFiles = dir.entryList(QDir::Files | QDir::NoSymLinks);
        QStringList matchingFiles;

        foreach(const QString& file, allFiles)
        {
            if (expr.exactMatch(file))
            {
                if (sendReciptFromFile(folderName + file))
                {
                    dir.remove(file);
                }
                else
                {
                    return;
                }
            }
        }
    }
}

bool MainWindow::sendReciptFromFile(QFile file)
{
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Can not open json file: " << file.fileName();
        return false;
    }

    QString data = file.readAll();
    file.close();

    Receipt       receipt;
    QJsonDocument doc    = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject   json   = doc.object();
    receipt.time         = json["time"].toInt();
    receipt.date         = json["data"].toString();
    receipt.numOfAzsNode = json["num_azs_node"].toInt();
    receipt.gasType      = json["gas_type"].toString();
    receipt.countLitres  = json["count_litres"].toString();
    receipt.sum          = json["sum"].toString();

    return sendReceipt(receipt);
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
        getResponseData().azsNodes[i].price   = azsNodes[i].price;
        getResponseData().azsNodes[i].gasType = azsNodes[i].gasType;
        azsNodeWidgets[i].gasTypeLable        = getGasTypeString(azsNodes[i].gasType);

        double price = static_cast<double>(azsNodes[i].price) / 100;

        azsNodeWidgets[i].pricePerLitreLable->setText(azsNodeWidgets[i].gasTypeLable +
                                                      QString(" %1").arg(price, 0, 'f', 2) + rubChar + "/Л");
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
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        double price = static_cast<double>(currentAzsNodes[i].price) / 100.f;
        double count = azsNodeWidgets[i].startBtn->isEnabled() ? balance / price : 0;
        azsNodeWidgets[i].countLitresLable->setText(QString("%1 Л").arg(count, 0, 'f', 2));
    }
}

void MainWindow::setVisibleSecondBtn(bool isVisible)
{
    constexpr int index = 1;
    azsNodeWidgets[index].pricePerLitreLable->setVisible(isVisible);
    azsNodeWidgets[index].startBtn->setVisible(isVisible);
    azsNodeWidgets[index].countLitresLable->setVisible(isVisible);
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

void MainWindow::setBtnFromServer(const AzsButton& azsButton)
{
    getResponseData().state = static_cast<ResponseData::State>(azsButton.button);
    if (azsButton.price1)
    {
        currentAzsNodes[0].price = azsButton.price1;
    }
    if (azsButton.price2)
    {
        currentAzsNodes[1].price = azsButton.price2;
    }
    if (azsButton.price1 || azsButton.price2)
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
        print.append("0x" + QString::number((uint8_t)b, 16) + " ");
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
        azsNodeWidgets[i].pricePerLitreLable = new Label(this);
        azsNodeWidgets[i].countLitresLable   = new Label(this);
        azsNodeWidgets[i].pricePerLitreLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        azsNodeWidgets[i].countLitresLable->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");

        azsNodeWidgets[i].startBtn = new QPushButton(this);
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
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        currentAzsNodes[i].price = settings.value("currentPrice" + QString::number(i)).toInt();
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

    receipt.time         = (int)QDateTime::currentSecsSinceEpoch();
    receipt.date         = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    receipt.numOfAzsNode = numOfAzsNode;
    receipt.gasType      = azsNodeWidgets[azsNodeIndex].gasTypeLable;
    receipt.countLitres  = azsNodeWidgets[azsNodeIndex].countLitresLable->text();
    receipt.sum          = balanceLable->text();
    return receipt;
}
