/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
