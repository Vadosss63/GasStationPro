#include "mainwindowcontroller.h"

#include <QApplication>
#include <QErrorMessage>
#include <QKeyEvent>
#include <QUrlQuery>

#include "appsettings.h"
#include "azsnodesettings.h"
#include "filesystemutilities.h"
#include "httprequest.h"
#include "logging.h"
#include "price.h"

MainWindowController::MainWindowController(QObject* parent) : QObject(parent)
{
    readSettings();
    readConfig();

    setCountAzsNodes(configure.activeBtn2);

    mainWindow.createWidget(configure.showSecondPrice);
    mainWindow.setSupportPhone(configure.phoneOfSupport);
    mainWindow.setVisibleSecondBtn(configure.activeBtn2, configure.showSecondPrice);
    mainWindow.setVisibleSecondBtn(configure.activeBtn2, configure.showSecondPrice);
    mainWindow.setKeyPressEvent(this);

    setAzsNode(currentAzsNodes);
    setBalance(0, 0);
    setCountOfLitres();

    serviceMenuController.createWindow(configure.showSecondPrice, countAzsNode);

    setupSecondPrice();

    ReceivedData data{};
    setShowData(data);

    connect(&port, SIGNAL(readyData()), this, SLOT(readDataFromPort()));

    /// TODO: linux
    /// grep -i 'tty' /var/log/dmesg
    port.setSettingsPort(QString(configure.comPort), configure.baudRate);
    port.connectPort();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendToServer()));
    timer->start(10000);

    connect(&serviceMenuController, SIGNAL(setPrice()), this, SLOT(setupPrice()));
    connect(&serviceMenuController, SIGNAL(readCounters()), this, SLOT(getCounters()));
    connect(&serviceMenuController, SIGNAL(resetCounters()), this, SLOT(resetCounters()));
    connect(&serviceMenuController, SIGNAL(showStatistics()), &receiptHistoryController, SLOT(showDialog()));

    connect(&mainWindow, SIGNAL(startFirstAzsNode()), this, SLOT(startFirstAzsNode()));
    connect(&mainWindow, SIGNAL(startFirstAzsNode()), this, SLOT(startSecondAzsNode()));
}

MainWindowController::~MainWindowController()
{
    timer->stop();
}

void MainWindowController::setShowData(const ReceivedData& data)
{
    setBalance(data.balanceCash, data.balanceCashless);
    setEnabledStart(data);
    setCountOfLitres();
}

void MainWindowController::setBalance(double inputBalanceCash, double inputBalanceCashless)
{
    balanceCash     = inputBalanceCash;
    balanceCashless = inputBalanceCashless;
    double price    = inputBalanceCash + inputBalanceCashless;
    mainWindow.setBalance(price);
    AppSettings::instance().getSettings().sum = price;
}

void MainWindowController::startFirstAzsNode()
{
    clickedFirstHWBtn();
    getResponseData().state = ResponseData::isPressedFirstBtn;
}

void MainWindowController::startSecondAzsNode()
{
    clickedSecondHWBtn();
    getResponseData().state = ResponseData::isPressedSecondBtn;
}

void MainWindowController::setupPrice()
{
    setAzsNode(serviceMenuController.getAzsNodes());
    writeSettings();
}

void MainWindowController::showMainWindow()
{
    mainWindow.showFullScreen();
}

void MainWindowController::keyPressEvent(QKeyEvent* event)
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
            receiptHistoryController.showDialog();
            break;
        case Qt::Key_Escape:
            qApp->exit(0);
            break;
        default:
            break;
    }
}

void MainWindowController::setupSecondPrice()
{
    if (!configure.showSecondPrice)
    {
        currentAzsNodes[firstNodeId].priceCashless  = 0;
        currentAzsNodes[secondNodeId].priceCashless = 0;
        setAzsNode(currentAzsNodes);
        writeSettings();
        setCountOfLitres();
    }
}

void MainWindowController::setCountOfLitres()
{
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        double priceCash     = Price::convertPriceToDouble(currentAzsNodes[nodeId].priceCash);
        double priceCashless = Price::convertPriceToDouble(currentAzsNodes[nodeId].priceCashless);

        bool isStartBtnEnabled = mainWindow.isStartBtnEnabled(nodeId);

        double countFuelCash     = (isStartBtnEnabled && priceCash) ? (balanceCash / priceCash) : 0;
        double countFuelCashless = (isStartBtnEnabled && priceCashless) ? (balanceCashless / priceCashless) : 0;

        double countFuel = countFuelCash + countFuelCashless;

        mainWindow.setCountLitres(countFuel, nodeId);
    }
}

void MainWindowController::sendToServer()
{
    const AzsButton azsButton = getServerBtn();
    if (azsButton.idAzs && azsButton.button)
    {
        setBtnFromServer(azsButton);
        resetServerBtn();
    }
    sendReport();
}

void MainWindowController::writeSettings()
{
    writeAzsNodeSettings(currentAzsNodes);
}

void MainWindowController::readSettings()
{
    currentAzsNodes = readAzsNodeSettings();
}

void MainWindowController::sendReport()
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("name", configure.name);
    params.addQueryItem("address", configure.address);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("count_colum", QString("%1").arg((int)(countAzsNode)));
    params.addQueryItem("is_second_price", QString("%1").arg((int)(configure.showSecondPrice)));

    ReceivedData receivedData = getReceivedData();

    params.addQueryItem("stats", receivedData.getJsonReport(countAzsNode, getResponseData().azsNodes));
    QString res = sendPost(configure.host + "/azs_stats", params).msg;
    printLogInf(res);
}

bool MainWindowController::resetServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);

    Answer answer = sendPost(configure.host + "/reset_azs_button", params);
    if (!answer.isOk)
    {
        printLogErr(answer.msg);
    }
    return answer.isOk;
}

void MainWindowController::setBtnFromServer(const AzsButton& azsButton)
{
    using State      = ResponseData::State;
    using PriceState = AzsButton::PriceState;

    switch (azsButton.button)
    {
        case PriceState::updatePriceCashForFirstNode:
            currentAzsNodes[firstNodeId].priceCash = azsButton.price;
            break;
        case PriceState::updatePriceCashForSecondNode:
            currentAzsNodes[secondNodeId].priceCash = azsButton.price;
            break;
        case PriceState::updatePriceCashlessFirstNode:
            currentAzsNodes[firstNodeId].priceCashless = azsButton.price;
            break;
        case PriceState::updatePriceCashlessSecondNode:
            currentAzsNodes[secondNodeId].priceCashless = azsButton.price;
            break;
        case State::blockAzsNode:
            [[fallthrough]];
        case State::unblockAzsNode:
            [[fallthrough]];
        case State::isPressedServiceBtn1:
            [[fallthrough]];
        case State::isPressedServiceBtn2:
            [[fallthrough]];
        case State::isPressedServiceBtn3:
            [[fallthrough]];
        case State::resetCounters:
            getResponseData().state = static_cast<ResponseData::State>(azsButton.button);
            break;
        default:
            return;
    }
    //TODO: to be refactored
    if (azsButton.price)
    {
        setAzsNode(currentAzsNodes);
        writeSettings();
    }

    if (State::blockAzsNode == azsButton.button)
    {
        mainWindow.disableAzs(true);
    }

    if (State::unblockAzsNode == azsButton.button)
    {
        mainWindow.disableAzs(false);
    }
}

void MainWindowController::setAzsNode(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    currentAzsNodes = azsNodes;
    for (size_t nodeId = 0; nodeId < azsNodes.size(); ++nodeId)
    {
        getResponseData().azsNodes[nodeId].priceCash     = azsNodes[nodeId].priceCash;
        getResponseData().azsNodes[nodeId].priceCashless = azsNodes[nodeId].priceCashless;
        getResponseData().azsNodes[nodeId].gasType       = azsNodes[nodeId].gasType;

        QString gasTypeStr = convertGasTypeToString(azsNodes[nodeId].gasType);

        mainWindow.setGasType(gasTypeStr, nodeId);

        double priceCash = Price::convertPriceToDouble(azsNodes[nodeId].priceCash);

        if (configure.showSecondPrice)
        {
            mainWindow.setPricePerLitreLableCash(priceCash, nodeId);

            double priceCashless = Price::convertPriceToDouble(azsNodes[nodeId].priceCashless);
            mainWindow.setPricePerLitreLableCashless(priceCashless, nodeId);
        }
        else
        {
            mainWindow.setPricePerLitreLable(priceCash, nodeId);
        }
    }
}

AzsButton MainWindowController::getServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    Answer    answer = sendPost(configure.host + "/get_azs_button", params);
    AzsButton azsButton;
    if (!answer.isOk)
    {
        printLogErr(answer.msg);
        return azsButton;
    }
    azsButton.readAzsButton(answer.msg);

    return azsButton;
}

void MainWindowController::setEnabledStart(const ReceivedData& showData)
{
    for (int nodeId = 0; nodeId < countAzsNodeMax; ++nodeId)
    {
        bool isEnabled = showData.getIsActiveBtn(nodeId);
        mainWindow.setEnabledStartBtn(isEnabled, nodeId);
    }
}

void MainWindowController::readConfig()
{
    QString filePath = "settings.json";

    Configure conf;
    if (!readConfigure(filePath, conf))
    {
        (new QErrorMessage(&mainWindow))->showMessage("The settings.json contains invalid fields!");
    }
    configure = conf;
}

void MainWindowController::setCountAzsNodes(bool isVisible)
{
    countAzsNode = isVisible ? 2 : 1;
}

bool MainWindowController::sendReceipt(const Receipt& receipt) const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("receipt", receipt.getReceiptJson());
    Answer answer = sendPost(configure.host + "/azs_receipt", params);
    printLogInf(answer.msg);

    return answer.isOk;
}

Receipt MainWindowController::fillReceipt(int numOfAzsNode) const
{
    const int azsNodeIndex = numOfAzsNode - 1;

    Receipt receipt{};

    receipt.time         = static_cast<int>(QDateTime::currentSecsSinceEpoch());
    receipt.date         = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    receipt.numOfAzsNode = numOfAzsNode;
    receipt.gasType      = mainWindow.getGasTypeStr(azsNodeIndex);
    receipt.countLitres  = mainWindow.getCountLitresStr(azsNodeIndex);
    receipt.sum          = mainWindow.getSumStr();
    return receipt;
}

void MainWindowController::readDataFromPort()
{
    QByteArray data = port.getData();
    printLogInf(data);

    ReceivedData* tmp = ReceivedData::getReceivedData(data);
    if (!tmp)
    {
        printLogInf(QString("ReceivedData is invalid"));
        return;
    }

    getReceivedData() = *tmp;
    updateStateOfBtn();

    setShowData(getReceivedData());
    sendToPort(getResponseData().getQByteArray());
    getResponseData().state = ResponseData::defaultVal;
}

void MainWindowController::updateStateOfBtn()
{
    using ClickedBtnState = ReceivedData::ClickedBtnState;
    switch (getReceivedData().isClickedBtn)
    {
        case ClickedBtnState::ShowServiceMode:
            showServiceMenu();
            break;
        case ClickedBtnState::CloseServiceMode:
            closeServiceMenu();
            break;
        case ClickedBtnState::Button1Pressed:
            clickedFirstHWBtn();
            break;
        case ClickedBtnState::Button2Pressed:
            clickedSecondHWBtn();
            break;
        default:
            break;
    }
}

void MainWindowController::showServiceMenu()
{
    serviceMenuController.showServiceMenu(getReceivedData(), currentAzsNodes);
}

void MainWindowController::closeServiceMenu()
{
    serviceMenuController.closeServiceMenu();
}

void MainWindowController::clickedFirstHWBtn() const
{
    constexpr int idexBtn = 1;
    saveReceipt(idexBtn);
}

void MainWindowController::clickedSecondHWBtn() const
{
    constexpr int idexBtn = 2;
    saveReceipt(idexBtn);
}

bool MainWindowController::isBalanceValid() const
{
    return AppSettings::instance().getSettings().sum > 0;
}

void MainWindowController::getCounters()
{
    serviceMenuController.setupReportTable(getReceivedData());
}

void MainWindowController::resetCounters()
{
    getResponseData().state = ResponseData::resetCounters;
}

void MainWindowController::sendReceiptFiles() const
{
    const QString     folderName      = AppSettings::instance().getReceiptFolderName();
    const QStringList listReciptFiles = getListReciptFiles();

    for (const QString& fileName : listReciptFiles)
    {
        const QString fileReceiptPath{folderName + fileName};
        if (!sendReciptFromFile(fileReceiptPath))
        {
            printLogErr(QString("Failed to send receipt from file"));
            return;
        }

        removeFile(fileReceiptPath);
    }
}

bool MainWindowController::sendReciptFromFile(const QString& fileReceiptPath) const
{
    std::optional<Receipt> receipt = readReceiptFromFile(fileReceiptPath);

    if (!receipt.has_value())
    {
        removeFile(fileReceiptPath);
        return false;
    }

    return sendReceipt(receipt.value());
}

void MainWindowController::saveReceipt(int numOfAzsNode) const
{
    sendReceiptFiles();

    if (!isBalanceValid())
    {
        return;
    }

    Receipt receipt = fillReceipt(numOfAzsNode);

    AppSettings::instance().addTextToLogFile(receipt.getReceipt());

    if (!sendReceipt(receipt))
    {
        writeReceiptToFile(receipt);
    }
}

void MainWindowController::sendToPort(const QString& data)
{
    port.writeToPort(data);
}

void MainWindowController::sendToPort(const QByteArray& data)
{
    port.writeToPort(data);
    printLogInf(data);
}

void MainWindowController::sendToPort(const std::string& data)
{
    sendToPort(QString::fromStdString(data));
}

ReceivedData& MainWindowController::getReceivedData()
{
    return receiveData;
}

ResponseData& MainWindowController::getResponseData()
{
    return sendData;
}
