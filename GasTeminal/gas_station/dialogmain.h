#ifndef DIALOGMAIN_H
#define DIALOGMAIN_H

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
#include "port.h"
#include "servicemenudialog.h"
#include "settingwindows.h"

class Label : public QLabel
{
    Q_OBJECT
public:
    explicit Label(QWidget* parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();

public slots:

protected:
    virtual void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE { emit clicked(); }
};

class DialogMain : public QWidget
{
    Q_OBJECT
public:
    DialogMain();
    ~DialogMain();

public slots:
    void showSettings();
    void startStation1();
    void startStation2();
    void setupPrice();

    void readDataFromPort();
    void printLog(QString log);
    void printLog(QByteArray data);

    void getCounters();
    void resetCounters();

protected:
    // действия при нажажатии клавиш
    void keyPressEvent(QKeyEvent* event) override;

private:
    SendData& getSendData();

    ReceiveData& getReceiveData();

    void    writeSettings();
    void    readSettings();
    QString getReceipt(int numCol);

    void setShowData(const ReceiveData& data);

    void setPriceLitres(std::array<float, countColum> prices);
    void setGasType(std::array<SendData::GasType, countColum> gasType);
    void setBalance(double price);
    void setCountLitres();

    void setVisibleBtn2(bool isVisible);

    void createWidget();
    void setEnableStart(const ReceiveData& showData);
    void saveReceiptBtn(int numCol);

    void sendToPort(const QString& data);
    void sendToPort(const QByteArray& data);
    void sendToPort(const std::string& data);

    void readConfig();

    SettingWindows*   m_settingWindows;
    ServiceMenuDialog m_serviceMenuDialog;
    Label*            m_balanceLable;
    Label*            m_phoneOfSupportLable;

    std::array<SendData::GasType, countColum> m_gasTypes{SendData::GasType::Gas92};
    std::array<QString, countColum>           m_gasTypeLables{nullptr};
    std::array<Label*, countColum>            m_priceLitresLable{nullptr};
    std::array<QPushButton*, countColum>      m_startPBs{nullptr};
    std::array<Label*, countColum>            m_countLitresLables{nullptr};
    std::array<float, countColum>             m_currentPrices = {0};

    Configure m_configure{};

    Port* m_port;

    double      m_balance = 0;
    ReceiveData receiveData;
    SendData    sendData;
};

#endif // DIALOGMAIN_H
