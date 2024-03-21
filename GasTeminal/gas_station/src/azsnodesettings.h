#pragma once

#include "dataprotocol.h"

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

    std::array<Node, countAzsNodeMax> nodes{};
};

void writeAzsNodeSettings(const AzsNodeSettings& azsNodes);

AzsNodeSettings readAzsNodeSettings();
