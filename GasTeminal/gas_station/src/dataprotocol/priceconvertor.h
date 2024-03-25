#pragma once

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
