#include "fuelvalueinputwidget.h"

#include <QHBoxLayout>
#include <QLabel>

FuelValueInputWidget::FuelValueInputWidget(int command, QWidget* parent) : InputWidget(command, parent)
{
    createWidget();
}

int FuelValueInputWidget::getValue() const
{
    return value->value();
}

void FuelValueInputWidget::setValue(int val)
{
    value->setValue(val);
}

void FuelValueInputWidget::createWidget()
{
    value = new QSpinBox;
    value->setRange(-100000, 100000);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(value);
    layout->addWidget(new QLabel("Л"));
}
