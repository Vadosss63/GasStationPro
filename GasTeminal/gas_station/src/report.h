#pragma once

#include "azsnodesettings.h"
#include "receiveddata.h"

struct AzsReport
{
    size_t          countNode{};
    QString         version{};
    bool            isBlock{false};
    ReceivedData    rec{};
    AzsNodeSettings azsNode{};
};

QString getJsonReport(const AzsReport& azsReport);
