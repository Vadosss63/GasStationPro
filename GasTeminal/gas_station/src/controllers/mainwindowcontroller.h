#pragma once

#include <QObject>
#include <QTimer>

#include "AzsButtonHandler.h"
#include "IKeyPressEvent.h"
#include "configure.h"
#include "dataprotocol.h"
#include "mainwindow.h"
#include "port.h"
#include "receipt.h"
#include "receipthistorycontroller.h"
#include "servicemenucontroller.h"
#include "webservercontroller.h"

class MainWindowController : public QObject, IKeyPressEvent, AzsBtnHandler
{
    Q_OBJECT
public:
    explicit MainWindowController(QObject* parent = nullptr);
    ~MainWindowController() override;

    void showMainWindow();

    void keyPressEvent(QKeyEvent* event) override;
    void setBtnFromServer(const AzsButton& azsButton) override;

public slots:
    void startFirstAzsNode();
    void startSecondAzsNode();
    void setupPrice();

    void getCounters();
    void resetCounters();

    void showServiceMenu();
    void readDataFromPort();

private slots:
    void sendToServer();

private:
    void setShowData(const ReceivedData& data);
    void setEnabledStart(const ReceivedData& showData);

    bool isBalanceValid() const;

    void clickedFirstHWBtn() const;
    void clickedSecondHWBtn() const;
    void updateStateOfBtn();
    void closeServiceMenu();

    void sendToPort(const QString& data);
    void sendToPort(const QByteArray& data);
    void sendToPort(const std::string& data);

    Receipt fillReceipt(int numOfAzsNode) const;
    void    saveReceipt(int numOfAzsNode) const;

    void setCountAzsNodes(bool isVisible);

    void setAzsNode(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);

    void writeSettings();
    void readSettings();

    void readConfig();

    void setupSecondPrice();
    void setCountOfLitres();

    void setBalance(double inputBalanceCash, double inputBalanceCashless);

    ResponseData& getResponseData();
    ReceivedData& getReceivedData();

    MainWindow mainWindow;

    QTimer* timer;

    std::array<ResponseData::AzsNode, countAzsNodeMax> currentAzsNodes{};

    Configure configure{};

    ReceivedData receiveData{};
    ResponseData sendData{};

    Port port;

    uint8_t countAzsNode{2};

    double balanceCashless{0};
    double balanceCash{0};

    ServiceMenuController    serviceMenuController;
    ReceiptHistoryController receiptHistoryController;
    WebServerController      webServerController;

    static constexpr size_t firstNodeId  = 0;
    static constexpr size_t secondNodeId = 1;
};
