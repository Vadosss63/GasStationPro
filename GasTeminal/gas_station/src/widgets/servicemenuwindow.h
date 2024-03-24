#pragma once

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QWidget>

#include "azsbuttonwidget.h"
#include "dataprotocol.h"

class ServiceMenuWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ServiceMenuWindow(QWidget* parent = nullptr);

    void createNodes(AzsButtonWidget* azsButtonWidget, size_t countNodes);

    void setCommonCash(uint32_t commonCash);
    void setDailyCash(uint32_t dailyCash);

    void setCommonCashless(uint32_t commonCashless);
    void setDailyCashless(uint32_t dailyCashless);

    void setCommonOnline(uint32_t commonOnline);
    void setDailyOnline(uint32_t dailyOnline);

    void setLiters(double common, double daily, size_t nodeId, size_t countAzsNode);

    void setVersion(const QString& ver);

signals:
    void pressedButton();
    void readCounters();
    void resetCounters();
    void showStatistics();

private:
    void createInfoTable(size_t countNodes);

    void setupButtons();
    void setupConnects();

    QPushButton*  setupBtn{nullptr};
    QPushButton*  countersBtn{nullptr};
    QPushButton*  resetCountersBtn{nullptr};
    QPushButton*  statisticsBtn{nullptr};
    QTableWidget* infoTable{nullptr};
    QLabel*       version{nullptr};
};
