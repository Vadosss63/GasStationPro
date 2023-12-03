#pragma once

#include "dataprotocol.h"

void writeAzsNodeSettings(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);
std::array<ResponseData::AzsNode, countAzsNodeMax> readAzsNodeSettings();
