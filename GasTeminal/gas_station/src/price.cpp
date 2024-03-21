#include "price.h"

Price::Price(uint16_t rub, uint16_t kop)
{
    this->rub = rub;
    this->kop = kop;
    priceInt  = rub * 100 + kop;
}

Price::Price(int priceInt)
{
    this->priceInt = priceInt;

    rub = (priceInt / 100) % 201;
    kop = priceInt % 100;
}

uint16_t Price::getRub() const
{
    return rub;
}

uint16_t Price::getKop() const
{
    return kop;
}

int Price::getPriceInt() const
{
    return priceInt;
}

double Price::convertPriceToDouble(uint32_t priceInt)
{
    return static_cast<double>(priceInt) / 100.;
}
