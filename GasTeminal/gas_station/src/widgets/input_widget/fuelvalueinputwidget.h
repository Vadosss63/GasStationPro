#pragma once

#include <QSpinBox>

#include "inputwidget.h"

class FuelValueInputWidget : public InputWidget
{
    Q_OBJECT
public:
    explicit FuelValueInputWidget(int command, QWidget* parent = nullptr);
    ~FuelValueInputWidget() override = default;

    int getValue() const override;

    void setValue(int val) override;

private:
    void createWidget();

    QSpinBox* value{nullptr};
};
