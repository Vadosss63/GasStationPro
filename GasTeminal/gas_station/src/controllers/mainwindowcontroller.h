#pragma once

#include "azsnodesettings.h"
#include "configure.h"
#include "constants.h"
#include "mainwindow.h"
#include "receipt.h"
#include "receiveddata.h"

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    MainWindowController(const Configure& configure, IKeyPressEvent* iKeyPressEvent, QObject* parent = nullptr);
    ~MainWindowController() override = default;

    void showMainWindow();

    void setAzsNode(const AzsNodeSettings& azsNodes, bool showSecondPrice);

    void disableAzs(bool state);

    bool isBlock();

    void setCountOfLitres(const AzsNodeSettings& currentAzsNodes);

    void setShowData(const ReceivedData& data);

    Receipt fillReceipt(int numOfAzsNode) const;

signals:
    void startFirstAzsNode();
    void startSecondAzsNode();

private:
    double getBalance() const;

    void setEnabledStart(const ReceivedData& showData);

    void setBalance(double inputBalanceCash, double inputBalanceCashless, double inputBalanceOnline);

    MainWindow mainWindow;

    double                              balanceCashless{0};
    double                              balanceCash{0};
    double                              balanceOnline{0};
    std::array<double, maxAzsNodeCount> countLitres{};
};
