#pragma once
#include <QGridLayout>
#include <QLabel>
#include <QMutex>
#include <QObject>
#include <QPushButton>
#include <QTimer>
#include <QWidget>
#include <condition_variable>
#include <thread>

#include "configure.h"
#include "dataprotocol.h"
#include "historyreceiptsdialog.h"
#include "port.h"
#include "servicemenuwindow.h"

class Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label(QWidget* parent = nullptr) : QLabel(parent) {}
    ~Label() override {}

signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE { emit clicked(); }
};

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow() override;

public slots:
    void showServiceMenu();
    void startFirstAzsNode();
    void startSecondAzsNode();
    void setupPrice();

    void readDataFromPort();
    void printLog(const QString& log);
    void printLog(const QByteArray& data);

    void getCounters();
    void resetCounters();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void createWidget();
    void writeSettings();
    void readSettings();
    void readConfig();

    void clickedFirstHWBtn();
    void clickedSecondHWBtn();
    void updateStateOfBtn();
    void closeServiceMenu();

    ResponseData& getResponseData();
    ReceivedData& getReceivedData();

    void setShowData(const ReceivedData& data);

    void setAzsNode(const std::array<ResponseData::AzsNode, countAzsNode>& azsNodes);
    void setBalance(double price);
    void setCountOfLitres();

    void setVisibleSecondBtn(bool isVisible);

    void setEnabledStart(const ReceivedData& showData);

    void    saveReceipt(int numOfAzsNode);
    QString getReceipt(int numOfAzsNode);

    void sendToPort(const QString& data);
    void sendToPort(const QByteArray& data);
    void sendToPort(const std::string& data);

    ServiceMenuWindow*     serviceMenuWindow{nullptr};
    HistoryReceiptsDialog* historyReceiptsDialog{nullptr};
    Label*                 balanceLable{nullptr};
    Label*                 phoneOfSupportLable{nullptr};

    std::array<ResponseData::AzsNode, countAzsNode> currentAzsNodes{};

    struct AzsNodeWidget
    {
        QString      gasTypeLable;
        Label*       pricePerLitreLable{nullptr};
        QPushButton* startBtn{nullptr};
        Label*       countLitresLable{nullptr};
    };

    std::array<AzsNodeWidget, countAzsNode> azsNodeWidgets{};

    Configure configure{};

    Port* port{nullptr};

    double       balance{0};
    ReceivedData receiveData{};
    ResponseData sendData{};
};
