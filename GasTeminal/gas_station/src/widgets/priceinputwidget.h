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

    int getValue() const override;

    void setValue(int val) override;

private:
    void createWidget();

    QSpinBox* priceRub{nullptr};
    QSpinBox* priceKop{nullptr};
};
