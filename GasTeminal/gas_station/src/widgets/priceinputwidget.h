#pragma once

#include <QObject>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "buttonwidget.h"
#include "price.h"

class PriceInputWidget : public ButtonWidget
{
    Q_OBJECT
public:
    explicit PriceInputWidget(int command, QWidget* parent = nullptr);
    ~PriceInputWidget() override = default;

    int getValue() const override
    {
        uint16_t rub = priceRub->value();
        uint16_t kop = priceKop->value();
        Price    priceCash(rub, kop);
        return priceCash.getPriceInt();
    }

    void setValue(int val) override
    {
        Price price{val};
        priceRub->setValue(price.getRub());
        priceKop->setValue(price.getKop());
    }

private:
    void createWidget()
    {
        priceRub = new QSpinBox;
        priceRub->setRange(0, 200);
        priceKop = new QSpinBox;
        priceKop->setRange(0, 99);

        auto layout = new QHBoxLayout(this);
        layout->addWidget(priceRub);
        layout->addWidget(priceKop);
    }

    QSpinBox* priceRub{nullptr};
    QSpinBox* priceKop{nullptr};
};
