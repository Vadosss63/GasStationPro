#pragma once

#include <QObject>
#include <QTimer>

#include "comportcontroller.h"
#include "configure.h"
#include "dataprotocol.h"
#include "mainwindow.h"
#include "port.h"
#include "receipt.h"
#include "receipthistorycontroller.h"
#include "servicemenucontroller.h"
#include "webservercontroller.h"

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    MainWindowController(const Configure& configure, IKeyPressEvent* iKeyPressEvent, QObject* parent = nullptr);
    ~MainWindowController() override;

    void showMainWindow();

    void setAzsNode(const AzsNodeSettings& azsNodes, bool showSecondPrice);

    void disableAzs(bool state);
    void setCountOfLitres(const AzsNodeSettings& currentAzsNodes);

    void setShowData(const ReceivedData& data);

    Receipt fillReceipt(int numOfAzsNode) const;

signals:
    void startFirstAzsNode();
    void startSecondAzsNode();

private:
    void setEnabledStart(const ReceivedData& showData);

    void setBalance(double inputBalanceCash, double inputBalanceCashless);

    MainWindow mainWindow;

    double balanceCashless{0};
    double balanceCash{0};
};
