#pragma once

#include <QByteArray>
#include <QString>
#include <stdint.h>

#pragma pack(push, 1)
struct ResponseData
{
    enum Command : uint8_t
    {
        defaultVal           = 0x00,
        isPressedServiceBtn1 = 0x01,
        isPressedServiceBtn2 = 0x02,
        isPressedServiceBtn3 = 0x03,
        resetCounters        = 0x10,
        blockAzsNode         = 0x11,
        unblockAzsNode       = 0x12,
        isPressedFirstBtn    = 0x20,
        isPressedSecondBtn   = 0x21,
        setGasType1          = 0x28,
        setGasType2          = 0x29,
        setPriceCash1        = 0x30,
        setPriceCash2        = 0x31,
        setPriceCashless1    = 0x38,
        setPriceCashless2    = 0x39,
        setLockFuelValue1    = 0x40,
        setLockFuelValue2    = 0x41,
        setFuelArrival1      = 0x48,
        setFuelArrival2      = 0x49
    };

    enum GasType : uint8_t
    {
        Gas92   = 0x01,
        Gas95   = 0x02,
        Gas98   = 0x03,
        DT      = 0x04,
        Methane = 0x05,
        Propane = 0x06
    };

    uint8_t  header{0x7F};
    Command  command{0x01};
    uint32_t data{0};
    uint8_t  checksum{0};

    QByteArray getQByteArray()
    {
        constexpr uint16_t size = sizeof(ResponseData);
        addChecksum(size);

        const char* ptr = reinterpret_cast<const char*>(this);

        return {ptr, size};
    }

private:
    void addChecksum(uint16_t size)
    {
        checksum     = 0;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(this);

        for (int i = 0; i < size - 1; ++i)
        {
            checksum += ptr[i];
        }
    }
};
#pragma pack(pop)

inline ResponseData::GasType convertIntToGasType(int type)
{
    return static_cast<ResponseData::GasType>(type);
}

inline QString convertGasTypeToString(ResponseData::GasType gasType)
{
    QString result;
    switch (gasType)
    {
        case ResponseData::Gas92:
            result = "АИ-92";
            break;
        case ResponseData::Gas95:
            result = "АИ-95";
            break;
        case ResponseData::Gas98:
            result = "АИ-98";
            break;
        case ResponseData::DT:
            result = "ДТ";
            break;
        case ResponseData::Methane:
            result = "Метан";
            break;
        case ResponseData::Propane:
            result = "Пропан";
            break;
    }
    return result;
}
