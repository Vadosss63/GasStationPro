#pragma once

#include <QTimer>

#include "configure.h"
#include "dataprotocol.h"
#include "labelwidget.h"
#include "port.h"
#include "receipt.h"
#include "receipthistorycontroller.h"
#include "receipthistorywindow.h"
#include "servicemenucontroller.h"
#include "temporarilyunavailablewidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    struct AzsNodeWidget
    {
        LabelWidget* gasTypeLable{nullptr};
        LabelWidget* pricePerLitreLableCash{nullptr};
        LabelWidget* pricePerLitreLableCashless{nullptr};
        QPushButton* startBtn{nullptr};
        LabelWidget* countLitresLable{nullptr};
    };

public:
    MainWindow();
    ~MainWindow() override;

public slots:
    void showServiceMenu();
    void startFirstAzsNode();
    void startSecondAzsNode();
    void setupPrice();

    void readDataFromPort();

    void getCounters();
    void resetCounters();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void sendToServer();

private:
    void      sendReport();
    bool      sendReceipt(const Receipt& receipt) const;
    AzsButton getServerBtn() const;
    bool      resetServerBtn() const;
    void      setBtnFromServer(const AzsButton& azsButton);
    void      disableAzs(bool disable);
    void      createWidget();
    void      writeSettings();
    void      readSettings();
    void      readConfig();
    void      setupSecondPrice();

    bool isBalanceValid() const;

    void clickedFirstHWBtn() const;
    void clickedSecondHWBtn() const;
    void updateStateOfBtn();
    void closeServiceMenu();

    ResponseData& getResponseData();
    ReceivedData& getReceivedData();

    void setShowData(const ReceivedData& data);

    void setAzsNode(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);
    void setBalance(double inputBalanceCash, double inputBalanceCashless);
    void setCountOfLitres();

    void setVisibleSecondBtn(bool isVisible);

    void setEnabledStart(const ReceivedData& showData);

    void    saveReceipt(int numOfAzsNode) const;
    Receipt fillReceipt(int numOfAzsNode) const;
    bool    sendReciptFromFile(QFile& fileReceipt) const;
    void    sendReceiptFiles() const;

    void sendToPort(const QString& data);
    void sendToPort(const QByteArray& data);
    void sendToPort(const std::string& data);
    void setCountAzsNodes(bool isVisible);

    ServiceMenuController    serviceMenuController;
    ReceiptHistoryController receiptHistoryController;

    TemporarilyUnavailableWidget* temporarilyUnavailableWidget;

    LabelWidget* balanceLable{nullptr};
    LabelWidget* phoneOfSupportLable{nullptr};

    std::array<ResponseData::AzsNode, countAzsNodeMax> currentAzsNodes{};

    std::array<AzsNodeWidget, countAzsNodeMax> azsNodeWidgets{};

    Configure configure{};

    Port*   port{nullptr};
    QTimer* timer;

    double       balanceCashless{0};
    double       balanceCash{0};
    uint8_t      countAzsNode{2};

    ReceivedData receiveData{};
    ResponseData sendData{};
};
