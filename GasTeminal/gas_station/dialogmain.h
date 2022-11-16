#ifndef DIALOGMAIN_H
#define DIALOGMAIN_H

#include <QGridLayout>
#include <QLabel>
#include <QMutex>
#include <QObject>
#include <QPushButton>
#include <QTimer>
#include <QWidget>

#include "dataprotocol.h"
#include "port.h"
#include "servicemenudialog.h"
#include "settingwindows.h"
#include "smtp.h"

class Label : public QLabel {
  Q_OBJECT
public:
  explicit Label(QWidget *parent = nullptr) : QLabel(parent) {}

signals:
  void clicked();

public slots:

protected:
  virtual void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE {
    emit clicked();
  }
};

struct SmtpSetting {
  QString smtpServer = "smtp.mail.ru";
  int serverPort = 465;
  QString email = "";
  QString password = "";
  int sendInHours = 0;
  bool isSend = false;
};

class DialogMain : public QWidget {
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

  void sendReport();

  void mailSent(QString status);
  void sendMail(QString msg);
  void showStatusMail(bool isOk);

signals:
  void sendReportToMail(QString msg);

protected:
  // действия при нажажатии клавиш
  void keyPressEvent(QKeyEvent *event) override;

private:
  SendData &getSendData();

  ReceiveData &getReceiveData();

  void writeSettings();
  void readSettings();
  QString getReceipt(int numCol);

  void setShowData(const ReceiveData &data);

  void setPriceLitres(std::array<float, countColum> prices);
  void setGasType(std::array<SendData::GasType, countColum> gasType);
  void setBalance(double price);
  void setCountLitres();

  void setVisibleBtn2(bool isVisible);

  void createWidget();
  void setEnableStart(const ReceiveData &showData);
  void saveReceiptBtn(int numCol);

  void settingTimerSend();

  void sendToPort(const QString &data) { m_port->writeToPort(data); }

  void sendToPort(const QByteArray &data) {
    m_port->writeToPort(data);
    printLog(data);
  }

  void sendToPort(const std::string &data) {
    sendToPort(QString::fromStdString(data));
  }
  void settingTouch();

  SettingWindows *m_settingWindows;
  ServiceMenuDialog m_serviceMenuDialog;
  Label *m_balanceLable;
  Label *m_phoneOfSupportLable;

  std::array<SendData::GasType, countColum> m_gasTypes{
      SendData::GasType::Gas92};
  std::array<QString, countColum> m_gasTypeLables{nullptr};
  std::array<Label *, countColum> m_priceLitresLable{nullptr};
  std::array<QPushButton *, countColum> m_startPBs{nullptr};
  std::array<Label *, countColum> m_countLitresLables{nullptr};
  std::array<float, countColum> m_currentPrices = {0};

  QString m_nameGasStation = "";
  QString m_phoneOfSupport = "8(999)000-00-00";
  bool m_isActiveBtn2 = true;
  Port *m_port;

  bool isShowMsgMailStatus = false;

  int m_sendReportInHours = 0;

  SmtpSetting m_smtpSetting;

  QTimer *m_timer;
  Smtp *smtp = nullptr;
  bool m_isSendToMail = false;
  bool m_isSendTestMail = true;
  double m_balance = 0;
  ReceiveData receiveData;
  SendData sendData;

  QMutex receiveDataMutex;
  QMutex sendDataMutex;
};

#endif // DIALOGMAIN_H
