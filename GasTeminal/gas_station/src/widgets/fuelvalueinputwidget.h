#pragma once

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "buttonwidget.h"

class FuelValueInputWidget : public ButtonWidget
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
