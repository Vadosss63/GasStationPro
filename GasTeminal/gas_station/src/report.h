#pragma once

#include "azsnodesettings.h"

struct AzsReport
{
    size_t          countNode;
    ReceivedData    rec;
    AzsNodeSettings azsNode;
};

QString getJsonReport(const AzsReport& azsReport);
