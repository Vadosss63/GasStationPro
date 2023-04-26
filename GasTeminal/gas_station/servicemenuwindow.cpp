#include "servicemenuwindow.h"

#include <QHeaderView>
#include <QStandardItemModel>

ServiceMenuWindow::ServiceMenuWindow(int showSecondPrice, QWidget* parent)
    : QWidget(parent, Qt::Tool), showSecondPrice(showSecondPrice)
{
    createWidget();
    QString style = "QPushButton, QSpinBox{"
                    "color: #111;"
                    "font-size: 50px;"
                    "background-color: qlineargradient( x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #4287ff, stop: 1 #356ccc);}"
                    "QPushButton:pressed {"
                    "color: #111;"
                    "background: #fff;"
                    "}"
                    "QComboBox{"
                    "color: #111;"
                    "background-color: #356ccc;"
                    "font-size: 50px;"
                    "}"
                    "QLabel {"
                    "color: #ddd;"
                    "font-size: 50px;"
                    "}";

    setStyleSheet(style);
}

ServiceMenuWindow::~ServiceMenuWindow() {}

void ServiceMenuWindow::setAzsNodes(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes)
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        int priceInt = azsNodes[i].priceCash;
        int rub      = (priceInt / 100) % 201;
        int kop      = priceInt % 100;
        azsNodeSettings[i].currentPriceCashRub->setValue(rub);
        azsNodeSettings[i].currentPriceCashKop->setValue(kop);

        if (showSecondPrice)
        {
            priceInt = azsNodes[i].priceCashless;
            rub      = (priceInt / 100) % 201;
            kop      = priceInt % 100;
            azsNodeSettings[i].currentPriceCashlessRub->setValue(rub);
            azsNodeSettings[i].currentPriceCashlessKop->setValue(kop);
        }

        azsNodeSettings[i].gasTypeCBs->setCurrentText(getGasTypeString(azsNodes[i].gasType));
    }
}

void ServiceMenuWindow::setupInfo(const ReceivedData& info, uint8_t countAzsNode)
{
    setTableReport(info, countAzsNode);
    bool isVisible = countAzsNode == 2;
    setVisibleSecondBtn(isVisible);
}

void ServiceMenuWindow::setTableReport(const ReceivedData& info, uint8_t countAzsNode)
{
    const size_t priceTableRows = infoTable->rowCount() - countAzsNode;

    infoTable->item(0, 0)->setData(Qt::DisplayRole, QVariant(info.commonCashSum + info.commonCoinsSum));
    infoTable->item(0, 1)->setData(Qt::DisplayRole, QVariant(info.dailyCashSum + info.dailyCoinsSum));

    infoTable->item(1, 0)->setData(Qt::DisplayRole, QVariant(info.commonCashlessSum));
    infoTable->item(1, 1)->setData(Qt::DisplayRole, QVariant(info.dailyCashlessSum));

    infoTable->item(2, 0)->setData(Qt::DisplayRole, QVariant(info.commonOnlineSum));
    infoTable->item(2, 1)->setData(Qt::DisplayRole, QVariant(info.dailyOnlineSum));

    for (int i = 0; i < countAzsNode; ++i)
    {
        infoTable->item(priceTableRows + i, 0)
            ->setData(Qt::DisplayRole,
                      QString("%1").arg(static_cast<double>(info.azsNodes[i].common) / 100., 0, 'f', 2));
        infoTable->item(priceTableRows + i, 1)
            ->setData(Qt::DisplayRole,
                      QString("%1").arg(static_cast<double>(info.azsNodes[i].daily) / 100., 0, 'f', 2));
    }
}

void ServiceMenuWindow::setupPrice()
{
    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        uint16_t rub          = azsNodeSettings[i].currentPriceCashRub->value();
        uint16_t kop          = azsNodeSettings[i].currentPriceCashKop->value();
        azsNodes[i].priceCash = rub * 100 + kop;

        if (showSecondPrice)
        {
            rub                       = azsNodeSettings[i].currentPriceCashlessRub->value();
            kop                       = azsNodeSettings[i].currentPriceCashlessKop->value();
            azsNodes[i].priceCashless = rub * 100 + kop;
        }

        int type            = azsNodeSettings[i].gasTypeCBs->currentData().toInt();
        azsNodes[i].gasType = static_cast<ResponseData::GasType>(type);
    }
    emit setPrice();
    close();
}

void ServiceMenuWindow::createWidget()
{
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
        azsNodeSettings[i].gasTypeCBs = new QComboBox;
        azsNodeSettings[i].gasTypeCBs->setModel(new QStandardItemModel);

        for (auto type : gasTypeMas)
        {
            azsNodeSettings[i].gasTypeCBs->addItem(getGasTypeString(type), static_cast<int>(type));
        }
        azsNodeSettings[i].currentPriceCashRub = new QSpinBox;
        azsNodeSettings[i].currentPriceCashRub->setRange(0, 200);
        azsNodeSettings[i].currentPriceCashKop = new QSpinBox;
        azsNodeSettings[i].currentPriceCashKop->setRange(0, 99);
        QHBoxLayout* hb            = new QHBoxLayout;
        azsNodeSettings[i].idLabel = new QLabel(QString::number(i + 1) + ":");
        hb->addWidget(azsNodeSettings[i].idLabel);
        hb->addWidget(azsNodeSettings[i].gasTypeCBs);

        if (showSecondPrice)
        {
            azsNodeSettings[i].currentPriceCashlessRub = new QSpinBox;
            azsNodeSettings[i].currentPriceCashlessRub->setRange(0, 200);
            azsNodeSettings[i].currentPriceCashlessKop = new QSpinBox;
            azsNodeSettings[i].currentPriceCashlessKop->setRange(0, 99);
            hb->addWidget(new QLabel("Наличн:"));
            hb->addWidget(azsNodeSettings[i].currentPriceCashRub);
            hb->addWidget(azsNodeSettings[i].currentPriceCashKop);
            hb->addWidget(new QLabel("Безнал:"));
            hb->addWidget(azsNodeSettings[i].currentPriceCashlessRub);
            hb->addWidget(azsNodeSettings[i].currentPriceCashlessKop);
        }
        else
        {
            hb->addWidget(azsNodeSettings[i].currentPriceCashRub);
            hb->addWidget(azsNodeSettings[i].currentPriceCashKop);
        }

        azsNodeSettings[i].azsLayout = new QWidget;
        azsNodeSettings[i].azsLayout->setLayout(hb);
    }

    gl->addWidget(azsNodeSettings[0].azsLayout, 1, 0, Qt::AlignCenter);
    gl->addWidget(azsNodeSettings[1].azsLayout, 2, 0, Qt::AlignCenter);
    constexpr int width = 280;

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
    createInfoTable();
    gl->addWidget(infoTable, 4, 0, Qt::AlignCenter);

    setupInfo(ReceivedData(), 2);
    setLayout(gl);
    setWindowTitle("Сервисное меню");
    connect(setupBtn, SIGNAL(clicked()), this, SLOT(setupPrice()));
    connect(countersBtn, SIGNAL(clicked()), this, SIGNAL(getCounters()));
    connect(resetCountersBtn, SIGNAL(clicked()), this, SIGNAL(resetCounters()));
    connect(statisticsBtn, SIGNAL(clicked()), this, SIGNAL(showStatistics()));
}

void ServiceMenuWindow::createInfoTable()
{
    const size_t priceTableRows = 3;
    const size_t tableRows      = priceTableRows + countAzsNodeMax;
    const size_t tableColoms    = 2;

    infoTable = new QTableWidget(tableRows, tableColoms);
    infoTable->setStyleSheet("color: #111; font: 30px 'Arial Black'; background-color: qlineargradient( x1: 0, y1: 0, "
                             "x2: 0, y2: 1, stop: 0 #4287ff, stop: 1 #356ccc);");
    infoTable->setFixedSize(610, 293);

    infoTable->setHorizontalHeaderItem(0, new QTableWidgetItem("общ"));
    infoTable->setHorizontalHeaderItem(1, new QTableWidgetItem("инкас"));

    infoTable->setVerticalHeaderItem(0, new QTableWidgetItem("Наличн.руб"));
    infoTable->setVerticalHeaderItem(1, new QTableWidgetItem("Безнал.руб"));
    infoTable->setVerticalHeaderItem(2, new QTableWidgetItem("Онлайн.руб"));

    for (size_t i = 0; i < tableRows; ++i)
    {
        for (size_t j = 0; j < tableColoms; ++j)
        {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            infoTable->setItem(i, j, item);
        }
    }

    QHeaderView* header = infoTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setDefaultSectionSize(200);

    header = infoTable->verticalHeader();
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setDefaultSectionSize(50);

    for (int i = 0; i < countAzsNodeMax; ++i)
    {
        infoTable->setVerticalHeaderItem(priceTableRows + i, new QTableWidgetItem(QString("%1-Литры").arg(i + 1)));
    }
}

void ServiceMenuWindow::setVisibleSecondBtn(bool isVisible)
{
    constexpr size_t idAzs = 1;
    azsNodeSettings[idAzs].azsLayout->setVisible(isVisible);
}

std::array<ResponseData::AzsNode, countAzsNodeMax> ServiceMenuWindow::getAzsNodes() const
{
    return azsNodes;
}
