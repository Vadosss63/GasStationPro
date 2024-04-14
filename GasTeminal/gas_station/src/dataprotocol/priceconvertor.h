#pragma once
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
#include <stdint.h>

class PriceConvertor
{
public:
    PriceConvertor(uint16_t rub, uint16_t kop);
    PriceConvertor(int priceInt);

    uint16_t getRub() const;

    uint16_t getKop() const;

    int getPriceInt() const;

    static double convertToDouble(uint32_t priceInt);

private:
    uint16_t rub{0};
    uint16_t kop{0};
    int      priceInt{0};
};
