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

    void setAzsNodes(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);

    void setupInfo(const ReceivedData& info, uint8_t countAzsNode);

    std::array<ResponseData::AzsNode, countAzsNodeMax> getAzsNodes() const;

signals:
    void setPrice();
    void getCounters();
    void resetCounters();
    void showStatistics();

public slots:

    void setupPrice();

private:
    void createWidget();
    void setVisibleSecondBtn(bool isVisible);

    QPushButton* setupBtn;
    QPushButton* countersBtn;
    QPushButton* resetCountersBtn;
    QPushButton* statisticsBtn;
    QLabel*      infoLable;

    std::array<QComboBox*, countAzsNodeMax> gasTypeCBs;
    std::array<QSpinBox*, countAzsNodeMax>  currentPriceRub;
    std::array<QSpinBox*, countAzsNodeMax>  currentPriceKop;
    std::array<QLabel*, countAzsNodeMax>    idLabel;

    std::array<ResponseData::AzsNode, countAzsNodeMax> azsNodes;
};
