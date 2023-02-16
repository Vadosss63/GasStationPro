#include "settingwindows.h"

#include <QStandardItemModel>

SettingWindows::SettingWindows(QWidget* parent) : QWidget(parent, Qt::Tool)
{
    createWidget();
    QString style = "QPushButton, QSpinBox, QComboBox{"
                    "color: #ddd;"
                    "font-size: 20px;"
                    "background-color: qlineargradient( x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #4287ff, stop: 1 #356ccc);}"
                    "QPushButton:pressed {"
                    "color: #111;"
                    "background: #fff;"
                    "}"
                    "QLabel {"
                    "color: #ddd;"
                    "font-size: 20px;"
                    "}";
    setStyleSheet(style);
}

SettingWindows::~SettingWindows() {}

void SettingWindows::setCurrentPrice(std::array<float, countColum> prices)
{
    for (int i = 0; i < countColum; ++i)
    {
        int priceInt = prices[i] * 100;
        int rub      = (priceInt / 100) % 100;
        int kop      = priceInt % 100;
        m_currentPriceRub[i]->setValue(rub);
        m_currentPriceKop[i]->setValue(kop);
    }
}

void SettingWindows::setGasTypes(std::array<SendData::GasType, countColum> gasTypes)
{
    for (int i = 0; i < countColum; ++i)
    {
        m_gasTypeSPs[i]->setCurrentText(getGasTypeString(gasTypes[i]));
    }
}
void SettingWindows::setupInfo(const ReceiveData& info)
{
    m_infoLable->setText("\n\n" + getTextReport(info));
}

void SettingWindows::setupPrice()
{
    for (int i = 0; i < countColum; ++i)
    {
        double rub   = m_currentPriceRub[i]->value();
        double kop   = m_currentPriceKop[i]->value();
        m_prices[i]  = rub + kop / 100;
        int type     = m_gasTypeSPs[i]->currentIndex();
        m_gasType[i] = gasType[type];
    }
    emit setPrice();
    close();
}

void SettingWindows::createWidget()
{
    std::array<QHBoxLayout*, countColum> hb;
    QGridLayout*                         gl = new QGridLayout;

    gasType = {SendData::GasType::DT,
               SendData::GasType::Gas92,
               SendData::GasType::Gas95,
               SendData::GasType::Gas98,
               SendData::GasType::Methane,
               SendData::GasType::Propane};

    gl->addWidget(new QLabel("Цена за литр"), 0, 0, 1, 3, Qt::AlignCenter);
    for (int i = 0; i < countColum; ++i)
    {
        m_gasTypeSPs[i] = new QComboBox;
        m_gasTypeSPs[i]->setModel(new QStandardItemModel);

        for (auto type : gasType)
        {
            m_gasTypeSPs[i]->addItem(getGasTypeString(type));
        }
        m_currentPriceRub[i] = new QSpinBox;
        m_currentPriceRub[i]->setRange(0, 250);
        m_currentPriceKop[i] = new QSpinBox;
        m_currentPriceKop[i]->setRange(0, 100);
        hb[i] = new QHBoxLayout;
        hb[i]->addWidget(new QLabel(QString::number(i + 1) + ":"));
        hb[i]->addWidget(m_gasTypeSPs[i]);
        hb[i]->addWidget(m_currentPriceRub[i]);
        hb[i]->addWidget(m_currentPriceKop[i]);
    }

    gl->addLayout(hb[0], 1, 0, Qt::AlignCenter);
    gl->addLayout(hb[1], 2, 0, Qt::AlignCenter);

    QHBoxLayout* hbl = new QHBoxLayout;
    m_pushButton     = new QPushButton("Установить");
    int w            = 140;
    m_pushButton->setFixedWidth(w);
    m_countersPushButton = new QPushButton("Счетчики");
    m_countersPushButton->setFixedWidth(w);
    m_resetCountersPushButton = new QPushButton("Инкассация");
    m_resetCountersPushButton->setFixedWidth(w);
    m_statPushButton = new QPushButton("Статистика");
    m_statPushButton->setFixedWidth(w);
    hbl->addWidget(m_pushButton);
    hbl->addWidget(m_countersPushButton);
    hbl->addWidget(m_resetCountersPushButton);
    hbl->addWidget(m_statPushButton);
    gl->addLayout(hbl, 3, 0, Qt::AlignCenter);
    m_infoLable = new QLabel;
    gl->addWidget(m_infoLable, 4, 0, Qt::AlignCenter);

    setupInfo(ReceiveData());
    setLayout(gl);
    setWindowTitle("Сервисное меню");

    connect(m_pushButton, SIGNAL(clicked()), this, SLOT(setupPrice()));
    connect(m_countersPushButton, SIGNAL(clicked()), this, SIGNAL(getCounters()));
    connect(m_resetCountersPushButton, SIGNAL(clicked()), this, SIGNAL(resetCounters()));
    connect(m_statPushButton, SIGNAL(clicked()), this, SIGNAL(showStat()));
}

std::array<SendData::GasType, countColum> SettingWindows::getGasType() const
{
    return m_gasType;
}

std::array<float, countColum> SettingWindows::getPrices() const
{
    return m_prices;
}
