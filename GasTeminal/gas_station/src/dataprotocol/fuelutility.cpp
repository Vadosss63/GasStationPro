#include "fuelutility.h"

double calculateFuelQuantity(double price, double balance)
{
    return price > 0 ? (balance / price) : 0;
}
