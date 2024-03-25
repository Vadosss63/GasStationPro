#pragma once

#include <QComboBox>

#include "inputwidget.h"

class GasTypeInputWidget : public InputWidget
{
    Q_OBJECT
public:
    explicit GasTypeInputWidget(int command, QWidget* parent = nullptr);
    ~GasTypeInputWidget() override = default;

    int getValue() const override;

    void setValue(int val) override;

private:
    void createWidget();

    QComboBox* gasTypeCBs{nullptr};
};
