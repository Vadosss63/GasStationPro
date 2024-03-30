#include "gastypeinputwidget.h"

#include <QHBoxLayout>
#include <QStandardItemModel>

#include "itemdelegate.h"
#include "responsedata.h"

GasTypeInputWidget::GasTypeInputWidget(int command, QWidget* parent) : InputWidget(command, parent)
{
    createWidget();
}

int GasTypeInputWidget::getValue() const
{
    return gasTypeCBs->currentData().toInt();
}

void GasTypeInputWidget::setValue(int val)
{
    QString gasType = convertGasTypeToString(static_cast<ResponseData::GasType>(val));
    gasTypeCBs->setCurrentText(gasType);
}

void GasTypeInputWidget::createWidget()
{
    const auto gasTypeMas = {ResponseData::GasType::DT,
                             ResponseData::GasType::Gas92,
                             ResponseData::GasType::Gas95,
                             ResponseData::GasType::Gas98,
                             ResponseData::GasType::Methane,
                             ResponseData::GasType::Propane};

    gasTypeCBs = new QComboBox;
    gasTypeCBs->setModel(new QStandardItemModel);

    ItemDelegate* delegate = new ItemDelegate(gasTypeCBs);
    gasTypeCBs->setItemDelegate(delegate);

    for (auto type : gasTypeMas)
    {
        gasTypeCBs->addItem(convertGasTypeToString(type), static_cast<int>(type));
    }

    auto layout = new QHBoxLayout(this);
    layout->addWidget(gasTypeCBs);
}
