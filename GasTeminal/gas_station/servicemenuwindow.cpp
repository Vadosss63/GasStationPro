#include "servicemenuwindow.h"

#include <QStandardItemModel>

ServiceMenuWindow::ServiceMenuWindow(QWidget* parent) : QWidget(parent, Qt::Tool)
{
    createWidget();
    QString style = "QPushButton, QSpinBox{"
                    "color: #111;"
                    "font-size: 20px;"
                    "background-color: qlineargradient( x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #4287ff, stop: 1 #356ccc);}"
                    "QPushButton:pressed {"
                    "color: #111;"
                    "background: #fff;"
                    "}"
                    "QComboBox{"
                    "color: #111;"
                    "background-color: #356ccc;"
                    "}"
                    "QLabel {"
                    "color: #ddd;"
                    "font-size: 20px;"
                    "}";
    setStyleSheet(style);
}

ServiceMenuWindow::~ServiceMenuWindow() {}

void ServiceMenuWindow::setAzsNodes(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        int priceInt = azsNodes[i].price;
        int rub      = (priceInt / 100) % 100;
        int kop      = priceInt % 100;
        currentPriceRub[i]->setValue(rub);
        currentPriceKop[i]->setValue(kop);

        gasTypeCBs[i]->setCurrentText(getGasTypeString(azsNodes[i].gasType));
    }
}

void ServiceMenuWindow::setupInfo(const ReceivedData& info, uint8_t countAzsNode)
{
    infoLable->setText("\n\n" + info.getTextReport(countAzsNode));
    bool isVisible = countAzsNode == 2;
    setVisibleSecondBtn(isVisible);
}

void ServiceMenuWindow::setupPrice()
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        uint16_t rub        = currentPriceRub[i]->value();
        uint16_t kop        = currentPriceKop[i]->value();
        azsNodes[i].price   = rub * 100 + kop;
        int type            = gasTypeCBs[i]->currentData().toInt();
        azsNodes[i].gasType = static_cast<ResponseData::GasType>(type);
    }
    emit setPrice();
    close();
}

void ServiceMenuWindow::createWidget()
{
    std::array<QHBoxLayout*, countAzsNodeMax> hb;

    QGridLayout* gl = new QGridLayout;

    auto gasTypeMas = {ResponseData::GasType::DT,
                       ResponseData::GasType::Gas92,
                       ResponseData::GasType::Gas95,
                       ResponseData::GasType::Gas98,
                       ResponseData::GasType::Methane,
                       ResponseData::GasType::Propane};

    gl->addWidget(new QLabel("Цена за литр"), 0, 0, 1, 3, Qt::AlignCenter);
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        gasTypeCBs[i] = new QComboBox;
        gasTypeCBs[i]->setModel(new QStandardItemModel);

        for (auto type : gasTypeMas)
        {
            gasTypeCBs[i]->addItem(getGasTypeString(type), static_cast<int>(type));
        }
        currentPriceRub[i] = new QSpinBox;
        currentPriceRub[i]->setRange(0, 250);
        currentPriceKop[i] = new QSpinBox;
        currentPriceKop[i]->setRange(0, 100);
        hb[i]      = new QHBoxLayout;
        idLabel[i] = new QLabel(QString::number(i + 1) + ":");
        hb[i]->addWidget(idLabel[i]);
        hb[i]->addWidget(gasTypeCBs[i]);
        hb[i]->addWidget(currentPriceRub[i]);
        hb[i]->addWidget(currentPriceKop[i]);
    }

    gl->addLayout(hb[0], 1, 0, Qt::AlignCenter);
    gl->addLayout(hb[1], 2, 0, Qt::AlignCenter);
    constexpr int width = 140;

    QHBoxLayout* hbl = new QHBoxLayout;
    setupBtn         = new QPushButton("Установить");
    setupBtn->setFixedWidth(width);
    countersBtn = new QPushButton("Счетчики");
    countersBtn->setFixedWidth(width);
    resetCountersBtn = new QPushButton("Инкассация");
    resetCountersBtn->setFixedWidth(width);
    statisticsBtn = new QPushButton("Статистика");
    statisticsBtn->setFixedWidth(width);
    hbl->addWidget(setupBtn);
    hbl->addWidget(countersBtn);
    hbl->addWidget(resetCountersBtn);
    hbl->addWidget(statisticsBtn);
    gl->addLayout(hbl, 3, 0, Qt::AlignCenter);
    infoLable = new QLabel;
    gl->addWidget(infoLable, 4, 0, Qt::AlignCenter);

    setupInfo(ReceivedData(), 2);
    setLayout(gl);
    setWindowTitle("Сервисное меню");

    connect(setupBtn, SIGNAL(clicked()), this, SLOT(setupPrice()));
    connect(countersBtn, SIGNAL(clicked()), this, SIGNAL(getCounters()));
    connect(resetCountersBtn, SIGNAL(clicked()), this, SIGNAL(resetCounters()));
    connect(statisticsBtn, SIGNAL(clicked()), this, SIGNAL(showStatistics()));
}

void ServiceMenuWindow::setVisibleSecondBtn(bool isVisible)
{
    constexpr size_t i = 1;
    gasTypeCBs[i]->setVisible(isVisible);
    currentPriceRub[i]->setVisible(isVisible);
    currentPriceKop[i]->setVisible(isVisible);
    idLabel[i]->setVisible(isVisible);
}

std::array<ResponseData::AzsNode, countAzsNodeMax> ServiceMenuWindow::getAzsNodes() const
{
    return azsNodes;
}
