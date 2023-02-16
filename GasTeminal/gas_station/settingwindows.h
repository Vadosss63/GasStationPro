#ifndef SETTINGWINDOWS_H
#define SETTINGWINDOWS_H

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include "dataprotocol.h"

class SettingWindows : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWindows(QWidget* parent = nullptr);
    ~SettingWindows();

    void setCurrentPrice(std::array<float, countColum> prices);
    void setGasTypes(std::array<SendData::GasType, countColum> gasTypes);

    void setupInfo(const ReceiveData& info);

    std::array<float, countColum>             getPrices() const;
    std::array<SendData::GasType, countColum> getGasType() const;

signals:
    void setPrice();
    void getCounters();
    void resetCounters();
    void showStat();

public slots:

    void setupPrice();

private:
    void createWidget();

    std::array<QComboBox*, countColum>        m_gasTypeSPs;
    std::array<QSpinBox*, countColum>         m_currentPriceRub;
    std::array<QSpinBox*, countColum>         m_currentPriceKop;
    QPushButton*                              m_pushButton;
    QPushButton*                              m_countersPushButton;
    QPushButton*                              m_resetCountersPushButton;
    QPushButton*                              m_statPushButton;
    QLabel*                                   m_infoLable;
    std::array<SendData::GasType, 6>          gasType;
    std::array<float, countColum>             m_prices;
    std::array<SendData::GasType, countColum> m_gasType;
};

#endif // SETTINGWINDOWS_H
