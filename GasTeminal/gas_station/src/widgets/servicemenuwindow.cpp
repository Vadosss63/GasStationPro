#include "servicemenuwindow.h"

#include <QHeaderView>
#include <QStandardItemModel>

ServiceMenuWindow::ServiceMenuWindow(QWidget* parent) : QWidget(parent, Qt::Tool)
{
    QString style = "QPushButton, QSpinBox{"
                    "color: #111;"
                    "font-size: 35px;"
                    "background-color: qlineargradient( x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #4287ff, stop: 1 #356ccc);}"
                    "QPushButton:pressed {"
                    "color: #111;"
                    "background: #fff;"
                    "}"
                    "QComboBox{"
                    "color: #111;"
                    "background-color: #356ccc;"
                    "font-size: 16px;"
                    "}"
                    "QLabel {"
                    "color: #ddd;"
                    "font-size: 35px;"
                    "}";

    setStyleSheet(style);
    setWindowTitle("Сервисное меню");
}

void ServiceMenuWindow::setupConnects()
{
    connect(setupBtn, SIGNAL(clicked()), this, SIGNAL(setPrice()));
    connect(countersBtn, SIGNAL(clicked()), this, SIGNAL(readCounters()));
    connect(resetCountersBtn, SIGNAL(clicked()), this, SIGNAL(resetCounters()));
    connect(statisticsBtn, SIGNAL(clicked()), this, SIGNAL(showStatistics()));
}

void ServiceMenuWindow::createOnePriceOneNode()
{
    constexpr size_t nodeId    = 0;
    constexpr size_t countNode = 1;

    setupGasTypeCB(nodeId);
    setupPriceCash(nodeId);
    setupAzsNodeId(nodeId);
    setupButtons();

    QGridLayout* gl = new QGridLayout;
    gl->addWidget(new QLabel("Цена за литр"), 0, 0, 1, 3, Qt::AlignCenter);
    QHBoxLayout* hb = new QHBoxLayout;

    hb->addWidget(azsNodeSettings[nodeId].idLabel);
    hb->addWidget(azsNodeSettings[nodeId].gasTypeCBs);

    hb->addWidget(azsNodeSettings[nodeId].currentPriceCashRub);
    hb->addWidget(azsNodeSettings[nodeId].currentPriceCashKop);

    azsNodeSettings[nodeId].azsLayout = new QWidget;
    azsNodeSettings[nodeId].azsLayout->setLayout(hb);

    gl->addWidget(azsNodeSettings[0].azsLayout, 1, 0, Qt::AlignCenter);
    gl->addWidget(azsNodeSettings[1].azsLayout, 2, 0, Qt::AlignCenter);

    QHBoxLayout* hbl = new QHBoxLayout;
    hbl->addWidget(setupBtn);
    hbl->addWidget(countersBtn);
    hbl->addWidget(resetCountersBtn);
    hbl->addWidget(statisticsBtn);

    gl->addLayout(hbl, 3, 0, Qt::AlignCenter);
    createInfoTable(countNode);

    gl->addWidget(infoTable, 4, 0, Qt::AlignCenter);

    setLayout(gl);
    setupConnects();
}

void ServiceMenuWindow::createOnePriceTwoNode()
{
    constexpr size_t countNode = countAzsNodeMax;

    QGridLayout* gl = new QGridLayout;
    gl->addWidget(new QLabel("Цена за литр"), 0, 0, 1, 3, Qt::AlignCenter);

    for (size_t nodeId = 0; nodeId < countNode; ++nodeId)
    {
        setupGasTypeCB(nodeId);
        setupPriceCash(nodeId);
        setupAzsNodeId(nodeId);

        QHBoxLayout* hb = new QHBoxLayout;
        hb->addWidget(azsNodeSettings[nodeId].idLabel);
        hb->addWidget(azsNodeSettings[nodeId].gasTypeCBs);
        hb->addWidget(azsNodeSettings[nodeId].currentPriceCashRub);
        hb->addWidget(azsNodeSettings[nodeId].currentPriceCashKop);

        azsNodeSettings[nodeId].azsLayout = new QWidget;
        azsNodeSettings[nodeId].azsLayout->setLayout(hb);
    }

    gl->addWidget(azsNodeSettings[0].azsLayout, 1, 0, Qt::AlignCenter);
    gl->addWidget(azsNodeSettings[1].azsLayout, 2, 0, Qt::AlignCenter);

    setupButtons();

    QHBoxLayout* hbl = new QHBoxLayout;

    hbl->addWidget(setupBtn);
    hbl->addWidget(countersBtn);
    hbl->addWidget(resetCountersBtn);
    hbl->addWidget(statisticsBtn);
    gl->addLayout(hbl, 3, 0, Qt::AlignCenter);
    createInfoTable(countNode);

    gl->addWidget(infoTable, 4, 0, Qt::AlignCenter);

    setLayout(gl);
    setupConnects();
}

void ServiceMenuWindow::createTwoPriceOneNode()
{
    constexpr size_t nodeId    = 0;
    constexpr size_t countNode = 1;

    setupGasTypeCB(nodeId);
    setupPriceCash(nodeId);
    setupAzsNodeId(nodeId);
    setupPriceCashless(nodeId);
    setupButtons();

    QGridLayout* gl = new QGridLayout;
    gl->addWidget(new QLabel("Цена за литр"), 0, 0, 1, 3, Qt::AlignCenter);

    QHBoxLayout* hb = new QHBoxLayout;
    hb->addWidget(azsNodeSettings[nodeId].idLabel);
    hb->addWidget(azsNodeSettings[nodeId].gasTypeCBs);

    hb->addWidget(new QLabel("Наличн:"));
    hb->addWidget(azsNodeSettings[nodeId].currentPriceCashRub);
    hb->addWidget(azsNodeSettings[nodeId].currentPriceCashKop);

    hb->addWidget(new QLabel("Безнал:"));
    hb->addWidget(azsNodeSettings[nodeId].currentPriceCashlessRub);
    hb->addWidget(azsNodeSettings[nodeId].currentPriceCashlessKop);

    azsNodeSettings[nodeId].azsLayout = new QWidget;
    azsNodeSettings[nodeId].azsLayout->setLayout(hb);

    gl->addWidget(azsNodeSettings[0].azsLayout, 1, 0, Qt::AlignCenter);
    gl->addWidget(azsNodeSettings[1].azsLayout, 2, 0, Qt::AlignCenter);

    QHBoxLayout* hbl = new QHBoxLayout;
    hbl->addWidget(setupBtn);
    hbl->addWidget(countersBtn);
    hbl->addWidget(resetCountersBtn);
    hbl->addWidget(statisticsBtn);
    gl->addLayout(hbl, 3, 0, Qt::AlignCenter);

    createInfoTable(countNode);

    gl->addWidget(infoTable, 4, 0, Qt::AlignCenter);

    setLayout(gl);
    setupConnects();
}

void ServiceMenuWindow::createTwoPriceTwoNode()
{
    constexpr size_t countNode = countAzsNodeMax;

    QGridLayout* gl = new QGridLayout;
    gl->addWidget(new QLabel("Цена за литр"), 0, 0, 1, 3, Qt::AlignCenter);

    for (size_t nodeId = 0; nodeId < countNode; ++nodeId)
    {
        setupGasTypeCB(nodeId);
        setupPriceCash(nodeId);
        setupAzsNodeId(nodeId);

        QHBoxLayout* hb = new QHBoxLayout;
        hb->addWidget(azsNodeSettings[nodeId].idLabel);
        hb->addWidget(azsNodeSettings[nodeId].gasTypeCBs);

        setupPriceCashless(nodeId);
        hb->addWidget(new QLabel("Наличн:"));
        hb->addWidget(azsNodeSettings[nodeId].currentPriceCashRub);
        hb->addWidget(azsNodeSettings[nodeId].currentPriceCashKop);
        hb->addWidget(new QLabel("Безнал:"));
        hb->addWidget(azsNodeSettings[nodeId].currentPriceCashlessRub);
        hb->addWidget(azsNodeSettings[nodeId].currentPriceCashlessKop);

        azsNodeSettings[nodeId].azsLayout = new QWidget;
        azsNodeSettings[nodeId].azsLayout->setLayout(hb);
    }

    gl->addWidget(azsNodeSettings[0].azsLayout, 1, 0, Qt::AlignCenter);
    gl->addWidget(azsNodeSettings[1].azsLayout, 2, 0, Qt::AlignCenter);

    setupButtons();

    QHBoxLayout* hbl = new QHBoxLayout;

    hbl->addWidget(setupBtn);
    hbl->addWidget(countersBtn);
    hbl->addWidget(resetCountersBtn);
    hbl->addWidget(statisticsBtn);
    gl->addLayout(hbl, 3, 0, Qt::AlignCenter);
    createInfoTable(countNode);

    gl->addWidget(infoTable, 4, 0, Qt::AlignCenter);

    setLayout(gl);
    setupConnects();
}

void ServiceMenuWindow::setCommonCash(uint32_t commonCash)
{
    infoTable->item(0, 0)->setData(Qt::DisplayRole, QVariant(commonCash));
}

void ServiceMenuWindow::setDailyCash(uint32_t dailyCash)
{
    infoTable->item(0, 1)->setData(Qt::DisplayRole, QVariant(dailyCash));
}

void ServiceMenuWindow::setCommonCashless(uint32_t commonCashless)
{
    infoTable->item(1, 0)->setData(Qt::DisplayRole, QVariant(commonCashless));
}

void ServiceMenuWindow::setDailyCashless(uint32_t dailyCashless)
{
    infoTable->item(1, 1)->setData(Qt::DisplayRole, QVariant(dailyCashless));
}

void ServiceMenuWindow::setCommonOnline(uint32_t commonOnline)
{
    infoTable->item(2, 0)->setData(Qt::DisplayRole, QVariant(commonOnline));
}

void ServiceMenuWindow::setDailyOnline(uint32_t dailyOnline)
{
    infoTable->item(2, 1)->setData(Qt::DisplayRole, QVariant(dailyOnline));
}

void ServiceMenuWindow::setLiters(double common, double daily, size_t nodeId, size_t countAzsNode)
{
    const size_t priceTableRows = infoTable->rowCount() - countAzsNode;

    const size_t shift = priceTableRows + nodeId;

    infoTable->item(shift, 0)->setData(Qt::DisplayRole, QString("%1").arg(common, 0, 'f', 2));
    infoTable->item(shift, 1)->setData(Qt::DisplayRole, QString("%1").arg(daily, 0, 'f', 2));
}

void ServiceMenuWindow::setupGasTypeCB(size_t nodeId)
{
    const auto gasTypeMas = {ResponseData::GasType::DT,
                             ResponseData::GasType::Gas92,
                             ResponseData::GasType::Gas95,
                             ResponseData::GasType::Gas98,
                             ResponseData::GasType::Methane,
                             ResponseData::GasType::Propane};

    azsNodeSettings[nodeId].gasTypeCBs = new QComboBox;
    azsNodeSettings[nodeId].gasTypeCBs->setModel(new QStandardItemModel);

    for (auto type : gasTypeMas)
    {
        azsNodeSettings[nodeId].gasTypeCBs->addItem(getGasTypeString(type), static_cast<int>(type));
    }
}

void ServiceMenuWindow::setupPriceCash(size_t nodeId)
{
    azsNodeSettings[nodeId].currentPriceCashRub = new QSpinBox;
    azsNodeSettings[nodeId].currentPriceCashRub->setRange(0, 200);
    azsNodeSettings[nodeId].currentPriceCashKop = new QSpinBox;
    azsNodeSettings[nodeId].currentPriceCashKop->setRange(0, 99);
}

void ServiceMenuWindow::setupAzsNodeId(size_t nodeId)
{
    azsNodeSettings[nodeId].idLabel = new QLabel(QString::number(nodeId + 1) + ":");
}

void ServiceMenuWindow::setupPriceCashless(size_t nodeId)
{
    azsNodeSettings[nodeId].currentPriceCashlessRub = new QSpinBox;
    azsNodeSettings[nodeId].currentPriceCashlessRub->setRange(0, 200);
    azsNodeSettings[nodeId].currentPriceCashlessKop = new QSpinBox;
    azsNodeSettings[nodeId].currentPriceCashlessKop->setRange(0, 99);
}

void ServiceMenuWindow::setupButtons()
{
    constexpr int width = 280;

    setupBtn = new QPushButton("Установить");
    setupBtn->setFixedWidth(width);

    countersBtn = new QPushButton("Счетчики");
    countersBtn->setFixedWidth(width);

    resetCountersBtn = new QPushButton("Инкассация");
    resetCountersBtn->setFixedWidth(width);

    statisticsBtn = new QPushButton("Статистика");
    statisticsBtn->setFixedWidth(width);
}

void ServiceMenuWindow::createInfoTable(size_t countNodes)
{
    const size_t priceTableRows = 3;
    const size_t tableColoms    = 2;

    const size_t tableRows = priceTableRows + countNodes;

    infoTable = new QTableWidget(tableRows, tableColoms);
    infoTable->setStyleSheet("color: #111; font: 25px 'Arial Black'; background-color: qlineargradient( x1: 0, y1: 0, "
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

    for (size_t i = 0; i < countNodes; ++i)
    {
        infoTable->setVerticalHeaderItem(priceTableRows + i, new QTableWidgetItem(QString("%1-Литры").arg(i + 1)));
    }
}

int ServiceMenuWindow::getGasType(size_t nodeId) const
{
    return azsNodeSettings[nodeId].gasTypeCBs->currentData().toInt();
}

int ServiceMenuWindow::getPriceCashRub(size_t nodeId) const
{
    return azsNodeSettings[nodeId].currentPriceCashRub->value();
}

int ServiceMenuWindow::getPriceCashKop(size_t nodeId) const
{
    return azsNodeSettings[nodeId].currentPriceCashKop->value();
}

int ServiceMenuWindow::getPriceCashlessRub(size_t nodeId) const
{
    return azsNodeSettings[nodeId].currentPriceCashlessRub->value();
}

void ServiceMenuWindow::setPriceCashRub(int rub, size_t nodeId)
{
    azsNodeSettings[nodeId].currentPriceCashRub->setValue(rub);
}

void ServiceMenuWindow::setPriceCashKop(int kop, size_t nodeId)
{
    azsNodeSettings[nodeId].currentPriceCashKop->setValue(kop);
}

void ServiceMenuWindow::setPriceCashlessRub(int rub, size_t nodeId)
{
    azsNodeSettings[nodeId].currentPriceCashlessRub->setValue(rub);
}

void ServiceMenuWindow::setPriceCashlessKop(int rub, size_t nodeId)
{
    azsNodeSettings[nodeId].currentPriceCashlessKop->setValue(rub);
}

void ServiceMenuWindow::setGasType(const QString& gasType, size_t nodeId)
{
    azsNodeSettings[nodeId].gasTypeCBs->setCurrentText(gasType);
}

int ServiceMenuWindow::getPriceCashlessKop(size_t nodeId) const
{
    return azsNodeSettings[nodeId].currentPriceCashlessKop->value();
}
