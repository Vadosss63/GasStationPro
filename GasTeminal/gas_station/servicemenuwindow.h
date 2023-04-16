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

    QPushButton* setupBtn{nullptr};
    QPushButton* countersBtn{nullptr};
    QPushButton* resetCountersBtn{nullptr};
    QPushButton* statisticsBtn{nullptr};
    QLabel*      infoLable{nullptr};

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

    std::array<AzsNodeSettings, countAzsNodeMax>       azsNodeSettings;
    std::array<ResponseData::AzsNode, countAzsNodeMax> azsNodes;
};
