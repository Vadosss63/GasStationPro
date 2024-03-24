#pragma once

#include <QPushButton>

#include "IKeyPressEvent.h"
#include "dataprotocol.h"
#include "labelwidget.h"
#include "temporarilyunavailablewidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    struct AzsNodeWidget
    {
        LabelWidget* gasTypeLable{nullptr};
        LabelWidget* pricePerLitreLableCash{nullptr};
        LabelWidget* pricePerLitreLableCashless{nullptr};
        QPushButton* startBtn{nullptr};
        LabelWidget* countLitresLable{nullptr};
    };

public:
    MainWindow();
    ~MainWindow() override;

    QString getSumStr() const;
    QString getCountLitresStr(size_t nodeId) const;

    QString getGasTypeStr(size_t nodeId) const;

    void setGasType(QString gasType, size_t nodeId);

    void setPricePerLitreLableCash(double priceCash, size_t nodeId);

    void setSupportPhone(const QString& phone);

    void createWidget(bool showSecondPrice);

    void disableAzs(bool disable);

    bool isBlock();

    void setPricePerLitreLable(double priceCash, size_t nodeId);
    void setPricePerLitreLableCashless(double price, size_t nodeId);

    void setVisibleSecondBtn(bool isVisible, bool showSecondPrice);

    void setBalance(double balance);

    void setCountLitres(double countFuel, size_t nodeId);
    bool isStartBtnEnabled(size_t nodeId);

    void setKeyPressEvent(IKeyPressEvent* newIKeyPressEvent);

    void setEnabledStartBtn(bool isEnabled, size_t nodeId);

signals:
    void startFirstAzsNode();
    void startSecondAzsNode();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    TemporarilyUnavailableWidget* temporarilyUnavailableWidget{nullptr};

    IKeyPressEvent* iKeyPressEvent{nullptr};

    LabelWidget* balanceLable{nullptr};
    LabelWidget* phoneOfSupportLable{nullptr};

    std::array<AzsNodeWidget, countAzsNodeMax> azsNodeWidgets{};
};
