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

    int getValue() const override { return gasTypeCBs->currentData().toInt(); }

    void setValue(int val) override
    {
        QString gasType = convertGasTypeToString(static_cast<ResponseData::GasType>(val));
        gasTypeCBs->setCurrentText(gasType);
    }

private:
    void createWidget()
    {
        const auto gasTypeMas = {ResponseData::GasType::DT,
                                 ResponseData::GasType::Gas92,
                                 ResponseData::GasType::Gas95,
                                 ResponseData::GasType::Gas98,
                                 ResponseData::GasType::Methane,
                                 ResponseData::GasType::Propane};

        gasTypeCBs = new QComboBox;
        gasTypeCBs->setModel(new QStandardItemModel);

        for (auto type : gasTypeMas)
        {
            gasTypeCBs->addItem(convertGasTypeToString(type), static_cast<int>(type));
        }

        auto layout = new QHBoxLayout(this);
        layout->addWidget(gasTypeCBs);
    }

    QComboBox* gasTypeCBs{nullptr};
};
