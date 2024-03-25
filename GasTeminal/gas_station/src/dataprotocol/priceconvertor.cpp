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
