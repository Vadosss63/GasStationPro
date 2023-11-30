#pragma once

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QWidget>

#include "dataprotocol.h"

class ServiceMenuWindow : public QWidget
{
    Q_OBJECT

    struct AzsNodeSettings
    {
        QComboBox* gasTypeCBs{nullptr};
        QSpinBox*  currentPriceCashRub{nullptr};
        QSpinBox*  currentPriceCashKop{nullptr};
        QSpinBox*  currentPriceCashlessRub{nullptr};
        QSpinBox*  currentPriceCashlessKop{nullptr};
        QLabel*    idLabel{nullptr};
        QWidget*   azsLayout{nullptr};
    };

public:
    explicit ServiceMenuWindow(QWidget* parent = nullptr);

    void createOnePriceOneNode();
    void createOnePriceTwoNode();
    void createTwoPriceOneNode();
    void createTwoPriceTwoNode();

    int getGasType(size_t nodeId) const;

    int getPriceCashRub(size_t nodeId) const;
    int getPriceCashKop(size_t nodeId) const;

    int getPriceCashlessRub(size_t nodeId) const;
    int getPriceCashlessKop(size_t nodeId) const;

    void setPriceCashRub(int rub, size_t nodeId);
    void setPriceCashKop(int kop, size_t nodeId);
    void setPriceCashlessRub(int rub, size_t nodeId);

    void setPriceCashlessKop(int rub, size_t nodeId);
    void setGasType(const QString& gasType, size_t nodeId);

    void setCommonCash(uint32_t commonCash);
    void setDailyCash(uint32_t dailyCash);

    void setCommonCashless(uint32_t commonCashless);
    void setDailyCashless(uint32_t dailyCashless);

    void setCommonOnline(uint32_t commonOnline);
    void setDailyOnline(uint32_t dailyOnline);

    void setLiters(double common, double daily, size_t nodeId, size_t countAzsNode);

signals:
    void setPrice();
    void readCounters();
    void resetCounters();
    void showStatistics();

private:
    void createInfoTable(size_t countNodes);

    void setupGasTypeCB(size_t nodeId);
    void setupPriceCash(size_t nodeId);
    void setupAzsNodeId(size_t nodeId);
    void setupPriceCashless(size_t nodeId);
    void setupButtons();
    void setupConnects();

    QPushButton*  setupBtn{nullptr};
    QPushButton*  countersBtn{nullptr};
    QPushButton*  resetCountersBtn{nullptr};
    QPushButton*  statisticsBtn{nullptr};
    QTableWidget* infoTable{nullptr};

    std::array<AzsNodeSettings, countAzsNodeMax> azsNodeSettings;
};
