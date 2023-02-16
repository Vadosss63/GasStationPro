#include "dialogmain.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDate>
#include <QErrorMessage>
#include <QFile>
#include <QKeyEvent>
#include <QSettings>
#include <QSpacerItem>
#include <QThread>
#include <iostream>

#include <qmap.h>

#include "settings.h"

DialogMain::DialogMain()
{
    readSettings();
    settingTouch();

    m_port = new Port();
    connect(m_port, SIGNAL(error_(QString)), this, SLOT(printLog(QString)));
    connect(m_port, SIGNAL(readyData()), this, SLOT(readDataFromPort()));

    // Слот - ввод настроек!
    /// TODO: linux
    ///  grep -i 'tty' /var/log/dmesg
    m_port->writeSettingsPort(QString(m_comPortName), m_baudRate);
    m_port->connectPort();

    m_settingWindows = new SettingWindows();
    createWidget();

    ReceiveData data;
    data.balance     = 0;
    data.isActiveBtn = 0;

    setShowData(data);
    m_phoneOfSupportLable->setText(m_phoneOfSupport);
    setVisibleBtn2(m_isActiveBtn2);

    sockThread = std::thread(&DialogMain::startServerSock, this);
}

DialogMain::~DialogMain()
{
    delete m_settingWindows;
    m_port->disconnectPort();
    delete m_port;
    cv.notify_all();

    serverSock.stopServer();
    if (sockThread.joinable()) sockThread.join();
}

void DialogMain::startServerSock()
{
    serverSock.startServerSock();
}

std::array<char, ServerSock::sizeOutBuff> DialogMain::parseDataSock(std::array<char, ServerSock::sizeInBuff>& in)
{
    QString     listData(in.data());
    QStringList data = listData.split(",");
    if (data.size() != 3)
    {
        return {"error"};
    }

    QStringList cmd = data[0].split(":");
    if (data.size() != 3 && cmd.at(0) != "cmd")
    {
        return {"error"};
    }

    if (cmd.at(1) == "1")
    {
        std::lock_guard<std::mutex>               ml(receiveDataMutex);
        std::string                               answer = getTextReport(getReceiveData()).toStdString();
        std::array<char, ServerSock::sizeOutBuff> res;
        std::copy(answer.c_str(), answer.c_str() + answer.size(), res.begin());
        return res;
    }

    if (cmd.at(1) == "0")
    {
        // cmd:0,post:1,sum:50
        QStringList post = data[1].split(":");
        QStringList sum  = data[2].split(":");
        if (post.at(0) == "post" && sum.at(0) == "sum")
        {
            int postNum = post.at(1).toInt();
            (void)postNum;
            uint32_t onlineSum = sum.at(1).toInt();
            {
                std::lock_guard<std::mutex> ml(sendDataMutex);
                getSendData().onlineSum = onlineSum;
            }

            static std::mutex            mtx;
            std::unique_lock<std::mutex> lck(mtx);
            if (cv.wait_for(lck, std::chrono::seconds(3)) == std::cv_status::timeout)
            {
                return {"timeout"};
                // return {"ok"};
            }

            std::lock_guard<std::mutex> ml(receiveDataMutex);
            if (getReceiveData().commonOnlineSum == onlineSum) return {"ok"};
        }
    }

    return {"error"};
}

QMap<QString, QString> parsingSetting(QString setting)
{
    QMap<QString, QString> settingMap;

    setting          = setting.remove(QChar(' '), Qt::CaseInsensitive).trimmed();
    setting          = setting.remove(QChar('\n'), Qt::CaseInsensitive);
    setting          = setting.remove(QChar('\r'), Qt::CaseInsensitive);
    QStringList rows = setting.split(QLatin1Char(';'), Qt::SkipEmptyParts);

    for (int row = 0; row < rows.size(); ++row)
    {
        QStringList fields = rows.at(row).split(QLatin1Char('='), Qt::SkipEmptyParts);
        if (fields.size() != 2) continue;

        settingMap[fields.at(0)] = fields.at(1);
    }

    return settingMap;
}

void DialogMain::settingTouch()
{
#ifndef QT_DEBUG
    QFile file("settings.ini");
#else
    QFile file("/home/vadosss63/MyProject/GasStationPro/GasTeminal/gas_station/"
               "settings/settings.ini");
#endif

    file.open(QFile::ReadOnly);
    if (!file.isOpen())
    {
        (new QErrorMessage(this))->showMessage("The setting.ini is not accessible");
        return;
    }

    QString setting = QString::fromUtf8(file.readAll());
    file.close();

    QStringList listFields{"Name", "PhoneOfSupport", "ActiveBtn2", "ComPort", "BaudRate"};

    QMap fields = parsingSetting(setting);

    if (fields.size() != listFields.size())
    {
        (new QErrorMessage(this))->showMessage("Setting.ini file contains incorrect number of fields!");
        return;
    }

    for (int i = 0; i < listFields.size(); ++i)
    {
        if (!fields.contains(listFields[i]))
        {
            (new QErrorMessage(this))->showMessage("The setting.ini doesn't contain " + listFields[i] + " field!");
            return;
        }
    }

    bool ok    = false;
    bool okRes = true;
    int  iter  = 0;

    m_nameGasStation = fields.value(listFields.at(iter++));
    okRes &= !m_nameGasStation.isEmpty();

    m_phoneOfSupport = fields.value(listFields.at(iter++));
    okRes &= !m_phoneOfSupport.isEmpty();

    m_isActiveBtn2 = fields.value(listFields.at(iter++)).toInt(&ok);
    okRes &= ok;

    m_comPortName = fields.value(listFields.at(iter++));
    okRes &= !m_comPortName.isEmpty();

    m_baudRate = fields.value(listFields.at(iter++)).toInt(&ok);
    okRes &= ok;

    if (!okRes)
    {
        (new QErrorMessage(this))->showMessage("The setting.ini contains invalid fields!");
        return;
    }
}

ReceiveData& DialogMain::getReceiveData()
{
    return receiveData;
}

SendData& DialogMain::getSendData()
{
    return sendData;
}

void DialogMain::showSettings()
{
    m_settingWindows->setCurrentPrice(m_currentPrices);
    m_settingWindows->setGasTypes(m_gasTypes);
    // m_settingWindows->close();
    m_settingWindows->show();
    m_settingWindows->setFocus();
    getCounters();
    m_settingWindows->setFocus();
}

void DialogMain::startStation1()
{
    saveReceiptBtn(1);
    {
        std::lock_guard<std::mutex> ml(sendDataMutex);
        getSendData().state = SendData::isPressedBtn1;
    }
}

void DialogMain::startStation2()
{
    saveReceiptBtn(2);
    {
        std::lock_guard<std::mutex> ml(sendDataMutex);
        getSendData().state = SendData::isPressedBtn2;
    }
}

void DialogMain::saveReceiptBtn(int numCol)
{
    QString receipt = getReceipt(numCol);
    Settings::instance().addTextToLogFile(receipt);
}

void DialogMain::sendToPort(const QString& data)
{
    m_port->writeToPort(data);
}

void DialogMain::sendToPort(const QByteArray& data)
{
    m_port->writeToPort(data);
    printLog(data);
}

void DialogMain::sendToPort(const std::string& data)
{
    sendToPort(QString::fromStdString(data));
}

void DialogMain::setupPrice()
{
    setGasType(m_settingWindows->getGasType());
    setPriceLitres(m_settingWindows->getPrices());
    writeSettings();
}

void DialogMain::setShowData(const ReceiveData& data)
{
    setBalance(data.balance + data.onlineSum);
    setEnableStart(data);
    setCountLitres();
}

void DialogMain::setPriceLitres(std::array<float, countColum> prices)
{
    assert(prices.size() == countColum);
    {
        std::lock_guard<std::mutex> ml(sendDataMutex);
        for (size_t i = 0; i < prices.size(); ++i)
        {
            double price            = prices[i];
            getSendData().prices[i] = static_cast<uint16_t>(price * 100);
            m_currentPrices[i]      = price;
            m_priceLitresLable[i]->setText(m_gasTypeLables[i] + QString(" %1").arg(price, 0, 'f', 2) + pubChar + "/Л");
        }
    }
}

void DialogMain::setGasType(std::array<SendData::GasType, countColum> gasType)
{
    assert(gasType.size() == countColum);
    {
        std::lock_guard<std::mutex> ml(sendDataMutex);
        for (size_t i = 0; i < gasType.size(); ++i)
        {
            auto type                 = gasType[i];
            m_gasTypes[i]             = type;
            getSendData().gasTypes[i] = type;
            m_gasTypeLables[i]        = getGasTypeString(type);
        }
    }
}

void DialogMain::setBalance(double price)
{
    m_balance = price;
    m_balanceLable->setText(QString("%1").arg(price, 0, 'f', 2) + pubChar);
    Settings::instance().getSettingsPrametrs().sum = price;
}

void DialogMain::setCountLitres()
{
    for (int i = 0; i < countColum; ++i)
    {
        double count = m_startPBs[i]->isEnabled() ? m_balance / m_currentPrices[i] : 0;
        m_countLitresLables[i]->setText(QString("%1 Л").arg(count, 0, 'f', 2));
    }
}

void DialogMain::setVisibleBtn2(bool isVisible)
{
    constexpr int index = 1;
    m_priceLitresLable[index]->setVisible(isVisible);
    m_startPBs[index]->setVisible(isVisible);
    m_countLitresLables[index]->setVisible(isVisible);
}

void DialogMain::readDataFromPort()
{
    QByteArray data = m_port->getData();
    printLog(data);

    if (data[0] != HEADER) return;

    if (data[1] != RECEIVE_DATA_CMD)
    {
        printLog(QString("Error recived CMD"));
        return;
    }

    ReceiveData* tmp = ReceiveData::getReceiveData(data);
    if (!tmp)
    {
        printLog(QString("ReceiveData is nullptr"));
        return;
    }
    {
        std::lock_guard<std::mutex> ml(receiveDataMutex);
        getReceiveData() = *tmp;
    }

    cv.notify_all();
    if (getReceiveData().isClickedBtn == 0xFF)
    {
        showSettings();
    }
    else if (getReceiveData().isClickedBtn == 0xFE)
    {
        m_settingWindows->close();
    }
    else
    {
        for (int i = 0; i < countColum; ++i)
        {
            if (getReceiveData().isClickedBtn == (i + 1))
            {
                saveReceiptBtn(i + 1);
            }
        }
    }
    {
        std::lock_guard<std::mutex> ml(sendDataMutex);
        setShowData(getReceiveData());
        sendToPort(getSendData().getQByteArray());
        getSendData().state = SendData::defaultVal;
    }
}

void DialogMain::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_1:
            showSettings();
            break;
        case Qt::Key_2:
            resetCounters();
            break;
        case Qt::Key_3:
            m_serviceMenuDialog.showDialog();
            break;
        case Qt::Key_Escape:
            qApp->exit(0);
            break;
        default:
            break;
    }
    QWidget::keyPressEvent(event);
}

void DialogMain::printLog(QString log)
{
    std::cout << log.toStdString() << std::endl;
}

void DialogMain::printLog(QByteArray data)
{
    QString print;
    foreach(auto b, data)
    {
        print.append("0x" + QString::number((uint8_t)b, 16) + " ");
    }
    printLog(print);
}

void DialogMain::getCounters()
{
    std::lock_guard<std::mutex> ml(receiveDataMutex);
    m_settingWindows->setupInfo(getReceiveData());
}

void DialogMain::resetCounters()
{
    {
        std::lock_guard<std::mutex> ml(sendDataMutex);
        getSendData().state = SendData::resetCounters;
    }
    //(new QErrorMessage(this))->showMessage("Выполняется инкассация!");
}

void DialogMain::createWidget()
{
    QString style = "QPushButton{padding:40px;"
                    "color: #ddd;"
                    "font-size: 100px;"
                    "border-radius: 2px;"
                    "border: 1px solid #3873d9;"
                    "background-color: qlineargradient( x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #4287ff, stop: 1 #356ccc);}"
                    "QPushButton:pressed {"
                    "color: #111;"
                    "border: 1px solid #3873d9;"
                    "background: #fff;"
                    "}"
                    "QPushButton:disabled {"
                    "color: #ddd;"
                    "background-color: qlineargradient( x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #777777, stop: 1 #999999);}"
                    "}";

    for (int i = 0; i < countColum; ++i)
    {
        m_priceLitresLable[i]  = new Label(this);
        m_countLitresLables[i] = new Label(this);
        m_priceLitresLable[i]->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");
        m_countLitresLables[i]->setStyleSheet("color: #003EC9; font: 30px 'Arial Black';");

        m_startPBs[i] = new QPushButton(this);
    }
    m_priceLitresLable[0]->setGeometry(16, 664, 335, 36);
    m_priceLitresLable[0]->setAlignment(Qt::AlignCenter);
    m_countLitresLables[0]->setGeometry(16, 664 + 36, 335, 36);
    m_countLitresLables[0]->setAlignment(Qt::AlignCenter);

    m_priceLitresLable[1]->setGeometry(918, 664, 335, 36);
    m_priceLitresLable[1]->setAlignment(Qt::AlignCenter);
    m_countLitresLables[1]->setGeometry(918, 664 + 36, 335, 36);
    m_countLitresLables[1]->setAlignment(Qt::AlignCenter);

    m_startPBs[0]->setGeometry(16, 720, 335, 299);
    m_startPBs[0]->setStyleSheet("QPushButton{background: #00000000 url(:/images/image/btn1.png);}"
                                 "QPushButton:disabled{"
                                 "background: #00000000 url(:/images/image/btn1_disable.png);"
                                 "}");

    m_startPBs[1]->setGeometry(918, 720, 335, 299);
    m_startPBs[1]->setStyleSheet("QPushButton{background: #00000000 url(:/images/image/btn2.png);}"
                                 "QPushButton:disabled{"
                                 "background: #00000000 url(:/images/image/btn2_disable.png);"
                                 "}");

    m_balanceLable = new Label(this);
    m_balanceLable->setStyleSheet("color: #003EC9; font: 210px 'Arial Black';");
    m_balanceLable->setGeometry(47, 315, 1178, 270);
    m_balanceLable->setAlignment(Qt::AlignCenter);

    m_phoneOfSupportLable = new Label(this);
    m_phoneOfSupportLable->setGeometry(386, 932, 506, 78);
    m_phoneOfSupportLable->setStyleSheet("color: #003EC9; font: 34px 'Arial Black';");
    m_phoneOfSupportLable->setAlignment(Qt::AlignCenter);

    setGasType(m_gasTypes);
    setPriceLitres(m_currentPrices);

    setBalance(0);
    setCountLitres();

    connect(m_startPBs[0], SIGNAL(clicked()), this, SLOT(startStation1()));
    connect(m_startPBs[1], SIGNAL(clicked()), this, SLOT(startStation2()));
    connect(m_settingWindows, SIGNAL(setPrice()), this, SLOT(setupPrice()));

    connect(m_settingWindows, SIGNAL(getCounters()), this, SLOT(getCounters()));
    connect(m_settingWindows, SIGNAL(resetCounters()), this, SLOT(resetCounters()));
    connect(m_settingWindows, SIGNAL(showStat()), &m_serviceMenuDialog, SLOT(showDialog()));
    QPixmap bkgnd(":/images/image/background.png");
    // bkgnd = bkgnd.scaled(QSize(1280, 1024), Qt::KeepAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
}

void DialogMain::writeSettings()
{
    QSettings settings("Gas Station");
    settings.beginGroup("parms");
    for (int i = 0; i < countColum; ++i)
    {
        settings.setValue("m_currentPrice" + QString::number(i), m_currentPrices[i]);
        settings.setValue("m_gasTypes" + QString::number(i), m_gasTypes[i]);
    }
    settings.endGroup();
}

void DialogMain::readSettings()
{
    QSettings settings("Gas Station");
    settings.beginGroup("parms");
    bool ok = false;
    for (int i = 0; i < countColum; ++i)
    {
        m_currentPrices[i] = settings.value("m_currentPrice" + QString::number(i)).toFloat();
        m_gasTypes[i] = static_cast<SendData::GasType>(settings.value("m_gasTypes" + QString::number(i)).toUInt(&ok));
    }
    settings.endGroup();
}

void DialogMain::setEnableStart(const ReceiveData& showData)
{
    for (int i = 0; i < countColum; ++i)
    {
        m_startPBs[i]->setEnabled(showData.getIsActiveBtn(i));
    }
}

QString DialogMain::getReceipt(int numCol)
{
    QDate currentDate = QDate::currentDate(); // возвращаем текущую дату
    QTime currentTime = QTime::currentTime(); // возвращаем текущее время

    QString date = currentDate.toString("dd.MM.yyyy");
    QString time = currentTime.toString("hh:mm");
    QString text = "Дата: ";
    text.append(date);
    text.append(' ');
    text.append(time);
    text.append('\n');
    text.append("Колонка: ");
    text.append(QString::number(numCol));
    text.append('\n');
    text.append("Бензин: ");
    text.append(m_gasTypeLables[numCol - 1]);
    text.append('\n');
    text.append("Литры: ");
    text.append(m_countLitresLables[numCol - 1]->text());
    text.append('\n');
    text.append("Сумма: ");
    text.append(m_balanceLable->text() + " руб");
    return text;
}
