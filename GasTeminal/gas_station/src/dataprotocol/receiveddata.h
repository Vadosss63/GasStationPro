#pragma once
/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <QString>
#include <stdint.h>

#include "constants.h"

#pragma pack(push, 1)
struct ReceivedData
{
    enum ClickedBtnState
    {
        Normal           = 0x00,
        Button1Pressed   = 0x01,
        Button2Pressed   = 0x02,
        Unblock          = 0xF0,
        Block            = 0xF1,
        CloseServiceMode = 0xFE,
        ShowServiceMode  = 0xFF
    };

    uint8_t head{0};    // 0x7F
    uint8_t command{0}; // 0x81

    uint8_t isClickedBtn{0};
    // Активность кнопок «старт»
    // (0 – кнопка не активна, 1 – кнопка активна)
    // 0 бит – колонка 1
    // 1 бит – колонка 2
    uint8_t isActiveBtn{3};
    //Внесенная сумма наличными, дискрет – 1 руб.
    uint16_t balanceCash{1000};
    //Внесенная сумма безналичными, дискрет – 1 руб.
    uint16_t balanceCashless{0};
    //Внесенная сумма онлайн, дискрет – 1 руб.
    uint16_t balanceOnline{0};

    //Купюры, общий счётчик, дискрет – 1 руб
    uint32_t commonCashSum{0};
    //Купюры, суточный счётчик, дискрет – 1 руб
    uint32_t dailyCashSum{0};

    //Монеты, общий счётчик, дискрет – 1 руб
    uint32_t commonCoinsSum{0};
    //Монеты, суточный счётчик, дискрет – 1 руб
    uint32_t dailyCoinsSum{0};

    //Безнал, общий счётчик, дискрет – 1 руб
    uint32_t commonCashlessSum{0};
    //Безнал, суточный счётчик, дискрет – 1 руб
    uint32_t dailyCashlessSum{0};

    //Онлайн, общий счётчик, дискрет – 1 руб
    uint32_t commonOnlineSum{0};
    //Онлайн, суточный счётчик, дискрет – 1 руб
    uint32_t dailyOnlineSum{0};

    struct AzsNode
    {
        //Колонка n общий счётчик литров, дискрет – 0,01 л.
        uint32_t common{0};
        //Колонка n суточный счётчик литров, дискрет – 0,01 л.
        uint32_t daily{0};
        //Колонка n объем топлива, м3, тип - float
        float fuelVolume{0};
        //Колонка n объем топлива, %, тип - float
        float fuelVolumePerc{0};
        //Колонка n плотность, т/м3, тип - float
        float density{0};
        //Колонка n средняя температура, °C, тип - float
        float averageTemperature{0};
    };
    std::array<AzsNode, maxAzsNodeCount> azsNodes;

    uint8_t checksum{0};

    bool getIsActiveBtn(uint8_t indexBtn) const
    {
        return indexBtn < maxAzsNodeCount ? isActiveBtn & (1 << indexBtn) : false;
    }

    static ReceivedData* getReceivedData(QByteArray& data)
    {
        ReceivedData* receivedData{nullptr};

        if ((data[0] != HEADER_DATA) or (data.size() != sizeof(ReceivedData)))
        {
            return receivedData;
        }
        receivedData = reinterpret_cast<ReceivedData*>(data.data());

        return receivedData;
    }
};
#pragma pack(pop)
