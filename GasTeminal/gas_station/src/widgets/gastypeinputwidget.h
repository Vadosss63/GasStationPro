#pragma once

#include <QComboBox>
#include <QObject>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

#include "buttonwidget.h"
#include "dataprotocol.h"

class GasTypeInputWidget : public ButtonWidget
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
