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

#include "priceinputwidget.h"

#include <QHBoxLayout>

#include "priceconvertor.h"

PriceInputWidget::PriceInputWidget(int command, QWidget* parent) : InputWidget(command, parent)
{
    createWidget();
}

int PriceInputWidget::getValue() const
{
    uint16_t       rub = priceRub->value();
    uint16_t       kop = priceKop->value();
    PriceConvertor priceCash(rub, kop);
    return priceCash.getPriceInt();
}

void PriceInputWidget::setValue(int val)
{
    PriceConvertor price{val};
    priceRub->setValue(price.getRub());
    priceKop->setValue(price.getKop());
}

void PriceInputWidget::createWidget()
{
    priceRub = new QSpinBox;
    priceRub->setRange(0, 200);
    priceKop = new QSpinBox;
    priceKop->setRange(0, 99);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(priceRub);
    layout->addWidget(priceKop);
}
