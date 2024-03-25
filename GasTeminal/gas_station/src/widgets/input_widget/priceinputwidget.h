#pragma once

#include <QSpinBox>

#include "inputwidget.h"

class PriceInputWidget : public InputWidget
{
    Q_OBJECT
public:
    explicit PriceInputWidget(int command, QWidget* parent = nullptr);
    ~PriceInputWidget() override = default;

    int getValue() const override;

    void setValue(int val) override;

private:
    void createWidget();

    QSpinBox* priceRub{nullptr};
    QSpinBox* priceKop{nullptr};
};
