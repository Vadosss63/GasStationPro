#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <array>
#include <stdint.h>

#include "logging.h"

constexpr uint8_t countAzsNodeMax = 2;

constexpr char HEADER_DATA       = 0x7F;
constexpr char RECEIVED_DATA_CMD = 0x81;

#pragma pack(push, 1)
struct ReceivedData
{
    enum ClickedBtnState
    {
        Normal           = 0x00,
        Button1Pressed   = 0x01,
        Button2Pressed   = 0x02,
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
    uint8_t isActiveBtn{0};
    //Внесенная сумма наличными, дискрет – 1 руб.
    uint16_t balanceCash{0};
    //Внесенная сумма безналичными, дискрет – 1 руб.
    uint16_t balanceCashless{0};

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
    std::array<AzsNode, countAzsNodeMax> azsNodes;

    uint8_t checksum{0};

    bool getIsActiveBtn(uint8_t indexBtn) const
    {
        return indexBtn < countAzsNodeMax ? isActiveBtn & (1 << indexBtn) : false;
    }

    static ReceivedData* getReceivedData(QByteArray& data)
    {
        ReceivedData* receivedData{nullptr};

        if ((data[0] != HEADER_DATA) or (data[1] != RECEIVED_DATA_CMD) or (data.size() != sizeof(ReceivedData)))
        {
            return receivedData;
        }
        receivedData = reinterpret_cast<ReceivedData*>(data.data());

        return receivedData;
    }
};
#pragma pack(pop)

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

struct AzsButton
{
    int idAzs{};
    int value{};
    int button{};

    bool readAzsButton(const QString& jsonText)
    {
        const QByteArray jsonData = QByteArray::fromStdString(jsonText.toStdString());
        QJsonDocument    document = QJsonDocument::fromJson(jsonData);

        if (document.isNull())
        {
            LOG_ERROR("Failed to parse JSON!");
            return false;
        }

        QJsonObject object = document.object();

        if (object.isEmpty() || !object.contains("id_azs") || !object.contains("value") || !object.contains("button"))
        {
            LOG_ERROR("Missing or invalid field(s)!");
            return false;
        }

        idAzs  = object["id_azs"].toInt();
        value  = object["value"].toInt();
        button = object["button"].toInt();
        return true;
    }
};
