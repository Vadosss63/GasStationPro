#pragma once
#include "dataprotocol.h"

class AzsBtnHandler
{
public:
    virtual void setBtnFromServer(const AzsButton& azsButton) = 0;
};
