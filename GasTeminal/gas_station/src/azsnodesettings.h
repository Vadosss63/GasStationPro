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
#include <array>

#include "constants.h"

struct AzsNodeSettings
{
    struct Node
    {
        uint32_t gasType;
        uint32_t priceCash;
        uint32_t priceCashless;
        uint32_t lockFuelValue;
        uint32_t fuelArrival;
    };

    std::array<Node, maxAzsNodeCount> nodes{};
};

void writeAzsNodeSettings(const AzsNodeSettings& azsNodes);

AzsNodeSettings readAzsNodeSettings();

bool readVisibleBtns();

void writeVisibleBtns(bool isVisible);
