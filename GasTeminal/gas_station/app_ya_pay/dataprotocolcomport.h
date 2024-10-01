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

constexpr size_t minPackageSize = 4;

inline uint8_t getChecksum(uint8_t* ptr, size_t size)
{
    uint8_t sum{};
    for (size_t i = 0; i < size; ++i)
    {
        sum += ptr[i];
    }
    return sum;
}

#pragma pack(push, 1)

struct HeaderCom
{
    static constexpr uint8_t headValue = 0x59;

    uint8_t head;
    uint8_t command;
    uint8_t size;
};

template <typename DataType>
struct Package
{
    static constexpr size_t size = sizeof(Package);

    HeaderCom heder{.head = HeaderCom::headValue, .command = DataType::cmd, .size = sizeof(DataType)};
    DataType  dataType;
    uint8_t   checksum;

    inline void addChecksum()
    {
        if (size < minPackageSize)
        {
            return;
        }
        size_t checksumIndex = size - 1;

        uint8_t* ptr = reinterpret_cast<uint8_t*>(this);

        checksum = getChecksum(ptr, checksumIndex);
    }

    inline QByteArray getData() { return {reinterpret_cast<char*>(this), size}; }
};

enum StatusResp : uint8_t
{
    ok = 0,
    invalidReq,
    serverError
};

struct OrderComReq
{
    static constexpr uint8_t cmd = 0x01;

    uint8_t columnId;
    uint8_t status;
};

struct OrderComResp
{
    static constexpr uint8_t cmd = 0x81;

    enum OrderType : uint8_t
    {
        NoOrder = 0,
        Money,
        Liters,
        FullTank,
    };
    uint8_t   columnId;
    OrderType orderType;
    float     orderVolume;
    float     priceFuel;
};

struct CanceledComReq
{
    static constexpr uint8_t cmd = 0x02;

    enum ReasonType : uint8_t
    {
        other = 0,
        invalidPrice,
    };

    uint8_t    columnId;
    ReasonType reason;
};

struct CanceledComResp
{
    static constexpr uint8_t cmd = 0x82;

    uint8_t columnId;
    uint8_t status;
};

struct AcceptComReq
{
    static constexpr uint8_t cmd = 0x03;

    uint8_t columnId;
};

struct AcceptComResp
{
    static constexpr uint8_t cmd = 0x83;

    uint8_t columnId;
    uint8_t status;
};

struct FuelingComReq
{
    static constexpr uint8_t cmd = 0x04;

    uint8_t columnId;
};

struct FuelingComResp
{
    static constexpr uint8_t cmd = 0x84;

    uint8_t columnId;
    uint8_t status;
};

struct CompletedComReq
{
    static constexpr uint8_t cmd = 0x05;

    uint8_t columnId;
    float   litres;
};

struct CompletedComResp
{
    static constexpr uint8_t cmd = 0x85;

    uint8_t columnId;
    uint8_t status;
};
#pragma pack(pop)

inline QString getReason(CanceledComReq::ReasonType reason)
{
    switch (reason)
    {
        case CanceledComReq::ReasonType::other:
            return "Other";
        case CanceledComReq::ReasonType::invalidPrice:
            return "Invalid price";
        default:
            return "Unknown";
    }
}

inline bool validateHead(uint8_t head)
{
    return head == HeaderCom::headValue;
}

inline bool validateChecksum(uint8_t* ptr, size_t size)
{
    if (size < minPackageSize)
    {
        return false;
    }
    size_t checksumIndex = size - 1;

    return ptr[checksumIndex] == getChecksum(ptr, checksumIndex);
}

inline size_t getDataSize(uint8_t* ptr)
{
    constexpr size_t sizeIndex = 3;
    return ptr[sizeIndex];
}

inline uint8_t getCmd(uint8_t* ptr)
{
    constexpr size_t cmdIndex = 1;
    return ptr[cmdIndex];
}
