#pragma once

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include "dataprotocol.h"

class ServiceMenuWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ServiceMenuWindow(QWidget* parent = nullptr);
    ~ServiceMenuWindow() override;

    void setAzsNodes(const std::array<ResponseData::AzsNode, countAzsNode>& azsNodes);

    void setupInfo(const ReceivedData& info);

    std::array<ResponseData::AzsNode, countAzsNode> getAzsNodes() const;

signals:
    void setPrice();
    void getCounters();
    void resetCounters();
    void showStatistics();

public slots:

    void setupPrice();

private:
    void createWidget();

    QPushButton* setupBtn;
    QPushButton* countersBtn;
    QPushButton* resetCountersBtn;
    QPushButton* statisticsBtn;
    QLabel*      infoLable;

    std::array<QComboBox*, countAzsNode> gasTypeCBs;
    std::array<QSpinBox*, countAzsNode>  currentPriceRub;
    std::array<QSpinBox*, countAzsNode>  currentPriceKop;

    std::array<ResponseData::AzsNode, countAzsNode> azsNodes;
};
