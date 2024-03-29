#include "priceinputwidget.h"

#include <QHBoxLayout>

#include "priceconvertor.h"

PriceInputWidget::PriceInputWidget(int command, QWidget* parent) : InputWidget(command, parent)
{
    createWidget();
}

int PriceInputWidget::getValue() const
{
    uint16_t       rub = priceRub->value();
    uint16_t       kop = priceKop->value();
    PriceConvertor priceCash(rub, kop);
    return priceCash.getPriceInt();
}

void PriceInputWidget::setValue(int val)
{
    PriceConvertor price{val};
    priceRub->setValue(price.getRub());
    priceKop->setValue(price.getKop());
}

void PriceInputWidget::createWidget()
{
    priceRub = new QSpinBox;
    priceRub->setRange(0, 200);
    priceKop = new QSpinBox;
    priceKop->setRange(0, 99);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(priceRub);
    layout->addWidget(priceKop);
}
