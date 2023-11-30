#pragma once

#include <stdint.h>

class Price
{
public:
    Price(uint16_t rub, uint16_t kop);
    Price(int priceInt);

    uint16_t getRub() const;

    uint16_t getKop() const;

    int getPriceInt() const;

private:
    uint16_t rub{0};
    uint16_t kop{0};
    int      priceInt{0};
};
