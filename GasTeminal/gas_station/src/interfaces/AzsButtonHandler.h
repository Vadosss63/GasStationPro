#pragma once
#include "azsbutton.h"

class AzsBtnHandler
{
public:
    virtual void handleAzsBtn(const AzsButton& azsButton) = 0;
};
