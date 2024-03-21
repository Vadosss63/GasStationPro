#include "tazs.h"

#include <QApplication>
#include <QErrorMessage>

#include "appsettings.h"
#include "azsnodesettings.h"
#include "logging.h"
#include "report.h"

Tazs::Tazs(QObject* parent) : QObject{parent}
{
    readSettings();
    readConfig();

    setCountAzsNodes(configure.activeBtn2);

    comPortController        = std::make_unique<ComPortController>(configure.comPort, configure.baudRate);
    mainWindowController     = std::make_unique<MainWindowController>(configure, this);
    serviceMenuController    = std::make_unique<ServiceMenuController>();
    webServerController      = std::make_unique<WebServerController>();
    receiptHistoryController = std::make_unique<ReceiptHistoryController>();

    serviceMenuController->createWindow(configure.showSecondPrice, countAzsNode);
    webServerController->setConfigure(configure);
    webServerController->setAzsBtnHandler(this);

    setAzsNode(currentAzsNodes);
    setupSecondPrice();

    connect(serviceMenuController.get(), SIGNAL(setPrice()), this, SLOT(setupPrice()));
    connect(serviceMenuController.get(), SIGNAL(readCounters()), this, SLOT(getCounters()));
    connect(serviceMenuController.get(), SIGNAL(resetCounters()), this, SLOT(resetCounters()));
    connect(serviceMenuController.get(), SIGNAL(showStatistics()), receiptHistoryController.get(), SLOT(showDialog()));
    connect(comPortController.get(), SIGNAL(readyData()), this, SLOT(updateData()));

    connect(mainWindowController.get(), SIGNAL(startFirstAzsNode()), this, SLOT(startFirstAzsNode()));
    connect(mainWindowController.get(), SIGNAL(startFirstAzsNode()), this, SLOT(startSecondAzsNode()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendToServer()));
    timer->start(10000);
}

Tazs::~Tazs()
{
    timer->stop();
}

void Tazs::sendToServer()
{
    ReceivedData receivedData = comPortController->getReceivedData();

    QString statistics = getJsonReport(countAzsNode, receivedData, currentAzsNodes);

    webServerController->sendToServer(statistics);
}

void Tazs::run()
{
    mainWindowController->showMainWindow();
}

void Tazs::readConfig()
{
    constexpr auto filePath = "settings.json";

    std::optional<Configure> conf = readConfigure(filePath);
    if (!conf)
    {
        constexpr auto errorMsg = "The settings.json contains invalid fields!";

        std::unique_ptr<QErrorMessage> errorMessage = std::make_unique<QErrorMessage>();
        LOG_ERROR(errorMsg);
        errorMessage->showMessage(errorMsg);
        return;
    }
    configure = conf.value();
}

void Tazs::readSettings()
{
    currentAzsNodes = readAzsNodeSettings();
}

void Tazs::setCountAzsNodes(bool isVisible)
{
    countAzsNode = isVisible ? 2 : 1;
}

void Tazs::setBtnFromServer(const AzsButton& azsButton)
{
    using State = ResponseData::Command;

    switch (azsButton.button)
    {
        case State::setPriceCash1:
            currentAzsNodes.nodes[firstNodeId].priceCash = azsButton.value;
            break;
        case State::setPriceCash2:
            currentAzsNodes.nodes[secondNodeId].priceCash = azsButton.value;
            break;
        case State::setPriceCashless1:
            currentAzsNodes.nodes[firstNodeId].priceCashless = azsButton.value;
            break;
        case State::setPriceCashless2:
            currentAzsNodes.nodes[secondNodeId].priceCashless = azsButton.value;
            break;
        case State::setFuelArrival1:
            currentAzsNodes.nodes[firstNodeId].fuelArrival = azsButton.value;
            break;
        case State::setFuelArrival2:
            currentAzsNodes.nodes[secondNodeId].fuelArrival = azsButton.value;
            break;
        case State::setLockFuelValue1:
            currentAzsNodes.nodes[firstNodeId].lockFuelValue = azsButton.value;
            break;
        case State::setLockFuelValue2:
            currentAzsNodes.nodes[secondNodeId].lockFuelValue = azsButton.value;
            break;
        case State::blockAzsNode:
            mainWindowController->disableAzs(true);
            break;
        case State::unblockAzsNode:
            mainWindowController->disableAzs(false);
            break;
        case State::isPressedServiceBtn1:
            [[fallthrough]];
        case State::isPressedServiceBtn2:
            [[fallthrough]];
        case State::isPressedServiceBtn3:
            [[fallthrough]];
        case State::resetCounters:
            break;
        default:
            LOG_ERROR(QString("Press unknown btn: %1").arg(azsButton.button));
            return;
    }

    comPortController->setCommand(static_cast<ResponseData::Command>(azsButton.button), azsButton.value);

    if (azsButton.value)
    {
        setAzsNode(currentAzsNodes);
        writeSettings();
    }

    LOG_INFO(QString("Recived button: %1, value:2%").arg(azsButton.button).arg(azsButton.value));
}

void Tazs::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_1:
            showServiceMenu();
            LOG_INFO("Pressed: show service mode");
            break;
        case Qt::Key_2:
            resetCounters();
            LOG_INFO("Pressed: reset counters");
            break;
        case Qt::Key_3:
            receiptHistoryController->showDialog();
            LOG_INFO("Pressed: history receipt");
            break;
        case Qt::Key_Escape:
            LOG_INFO("Pressed: exit app");
            qApp->exit(0);
            break;
        default:
            break;
    }
}

void Tazs::writeSettings()
{
    writeAzsNodeSettings(currentAzsNodes);
}

void Tazs::setupSecondPrice()
{
    if (!configure.showSecondPrice)
    {
        currentAzsNodes.nodes[firstNodeId].priceCashless  = 0;
        currentAzsNodes.nodes[secondNodeId].priceCashless = 0;
        setAzsNode(currentAzsNodes);
        writeSettings();

        mainWindowController->setCountOfLitres(currentAzsNodes);
    }
}

void Tazs::setAzsNode(const AzsNodeSettings& azsNodes)
{
    currentAzsNodes = azsNodes;
    ///TODO: Как установить все значения?
    //comPortController->getResponseData().azsNodes = azsNodes;

    mainWindowController->setAzsNode(azsNodes, configure.showSecondPrice);
}

void Tazs::updateData()
{
    updateStateOfBtn();
    const ReceivedData& data = comPortController->getReceivedData();
    mainWindowController->setShowData(data);
    mainWindowController->setCountOfLitres(currentAzsNodes);
    comPortController->sendResponse();
}

void Tazs::showServiceMenu()
{
    serviceMenuController->showServiceMenu(comPortController->getReceivedData(), currentAzsNodes);
}

void Tazs::closeServiceMenu()
{
    serviceMenuController->closeServiceMenu();
}

void Tazs::clickedFirstHWBtn() const
{
    constexpr int idexBtn = 1;
    saveReceipt(idexBtn);
}

void Tazs::clickedSecondHWBtn() const
{
    constexpr int idexBtn = 2;
    saveReceipt(idexBtn);
}

void Tazs::startFirstAzsNode()
{
    comPortController->setCommand(ResponseData::isPressedFirstBtn);
}

void Tazs::startSecondAzsNode()
{
    comPortController->setCommand(ResponseData::isPressedSecondBtn);
}

void Tazs::setupPrice()
{
    setAzsNode(serviceMenuController->getAzsNodes());
    writeSettings();
}

void Tazs::updateStateOfBtn()
{
    using ClickedBtnState = ReceivedData::ClickedBtnState;
    switch (comPortController->getReceivedData().isClickedBtn)
    {
        case ClickedBtnState::ShowServiceMode:
            showServiceMenu();
            LOG_INFO("Pressed: show service mode");
            break;
        case ClickedBtnState::CloseServiceMode:
            closeServiceMenu();
            LOG_INFO("Pressed: close service mode");
            break;
        case ClickedBtnState::Button1Pressed:
            clickedFirstHWBtn();
            LOG_INFO("Pressed: button 1");
            break;
        case ClickedBtnState::Button2Pressed:
            clickedSecondHWBtn();
            LOG_INFO("Pressed: button 2");
            break;
        default:
            LOG_ERROR(
                QString("Pressed: unknown %1 button").arg((int)comPortController->getReceivedData().isClickedBtn));
            break;
    }
}

bool Tazs::isBalanceValid() const
{
    return AppSettings::instance().getSettings().sum > 0;
}

void Tazs::getCounters()
{
    serviceMenuController->setupReportTable(comPortController->getReceivedData());
}

void Tazs::resetCounters()
{
    comPortController->setCommand(ResponseData::resetCounters);
}

void Tazs::saveReceipt(int numOfAzsNode) const
{
    webServerController->sendReceiptFiles();

    if (!isBalanceValid())
    {
        LOG_ERROR(QString("Balance is 0 for %1 azsNode").arg(numOfAzsNode));
        return;
    }

    Receipt receipt = mainWindowController->fillReceipt(numOfAzsNode);

    AppSettings::instance().addTextToLogFile(receipt.getReceipt());

    if (!webServerController->sendReceipt(receipt))
    {
        LOG_WARNING(QString("Receipt:%1 isn't sent. It'll be written to file").arg(receipt.time));
        writeReceiptToFile(receipt);
    }
}
