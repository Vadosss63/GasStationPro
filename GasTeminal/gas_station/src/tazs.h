#pragma once
#include <QObject>
#include <QTimer>

#include "azsbuttonhandler.h"
#include "azsnodesettings.h"
#include "comportcontroller.h"
#include "ikeypressevent.h"
#include "mainwindowcontroller.h"
#include "receipthistorycontroller.h"
#include "report.h"
#include "servicemenucontroller.h"
#include "webservercontroller.h"

class Tazs : public QObject, public AzsBtnHandler, public IKeyPressEvent
{
    Q_OBJECT
public:
    explicit Tazs(QObject* parent = nullptr);
    ~Tazs();

    void run();

    void handleAzsBtn(const AzsButton& azsButton) override;
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void startFirstAzsNode();
    void startSecondAzsNode();
    void setupPrice();
    void showServiceMenu();
    void updateData();

    void getCounters();

    void resetCounters();
private slots:
    void sendToServer();

private:
    void readConfig();

    void readSettings();
    void writeSettings();

    void setCountAzsNodes(bool isVisible);

    void setupSecondPrice();

    void setAzsNode(const AzsNodeSettings& azsNodes);

    void updateStateOfBtn(uint8_t isClickedBtn);

    void clickedFirstHWBtn() const;
    void clickedSecondHWBtn() const;
    void closeServiceMenu();

    void syncAzs();

    void      saveReceipt(int numOfAzsNode) const;
    bool      isBalanceValid() const;
    AzsReport getReport() const;

    std::unique_ptr<MainWindowController>     mainWindowController{nullptr};
    std::unique_ptr<ComPortController>        comPortController{nullptr};
    std::unique_ptr<ServiceMenuController>    serviceMenuController{nullptr};
    std::unique_ptr<ReceiptHistoryController> receiptHistoryController{nullptr};
    std::unique_ptr<WebServerController>      webServerController{nullptr};

    QTimer* timer;

    Configure configure{};
    uint8_t   countAzsNode{2};

    AzsNodeSettings currentAzsNodes{};
};
