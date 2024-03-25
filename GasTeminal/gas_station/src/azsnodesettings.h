#pragma once

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
