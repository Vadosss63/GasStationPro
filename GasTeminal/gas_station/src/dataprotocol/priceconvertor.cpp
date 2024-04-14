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

#include "priceconvertor.h"

PriceConvertor::PriceConvertor(uint16_t rub, uint16_t kop)
{
    this->rub = rub;
    this->kop = kop;
    priceInt  = rub * 100 + kop;
}

PriceConvertor::PriceConvertor(int priceInt)
{
    this->priceInt = priceInt;

    rub = (priceInt / 100) % 201;
    kop = priceInt % 100;
}

uint16_t PriceConvertor::getRub() const
{
    return rub;
}

uint16_t PriceConvertor::getKop() const
{
    return kop;
}

int PriceConvertor::getPriceInt() const
{
    return priceInt;
}

double PriceConvertor::convertToDouble(uint32_t priceInt)
{
    return static_cast<double>(priceInt) / 100.;
}
