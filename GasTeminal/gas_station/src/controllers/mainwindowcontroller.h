#pragma once

#include <QObject>
#include <QTimer>

#include "IKeyPressEvent.h"
#include "configure.h"
#include "dataprotocol.h"
#include "mainwindow.h"
#include "port.h"
#include "receipt.h"
#include "receipthistorycontroller.h"
#include "servicemenucontroller.h"

class MainWindowController : public QObject, IKeyPressEvent
{
    Q_OBJECT
public:
    explicit MainWindowController(QObject* parent = nullptr);
    ~MainWindowController() override;

    void showMainWindow();

    void keyPressEvent(QKeyEvent* event) override;

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

    bool sendReceipt(const Receipt& receipt) const;

    void setAzsNode(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);

    void writeSettings();
    void readSettings();

    void readConfig();

    void      sendReport();
    AzsButton getServerBtn() const;

    void setBtnFromServer(const AzsButton& azsButton);

    bool resetServerBtn() const;

    void sendReceiptFiles() const;

    bool sendReciptFromFile(const QString& fileReceipt) const;

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

    static constexpr size_t firstNodeId  = 0;
    static constexpr size_t secondNodeId = 1;
};
