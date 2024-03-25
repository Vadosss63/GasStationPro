#include "volumeconvertor.h"

double convertIntToLiter(uint32_t val)
{
    return static_cast<double>(val) / 100.;
}
