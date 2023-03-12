#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <array>
#include <stdint.h>

constexpr uint8_t countAzsNodeMax = 2;
constexpr uint8_t countAzsNode    = 2;

constexpr char HEADER_DATA       = 0x7F;
constexpr char RECEIVED_DATA_CMD = 0x81;

// inline QChar rubChar = QChar(0x20BD);
inline QChar rubChar = QString("Р").at(0);

#pragma pack(push, 1)
struct ReceivedData
{
    uint8_t head{0};    // 0x7F
    uint8_t command{0}; // 0x81
    // Состояние
    // 0x00 – обычное состояние,
    // 0x01 – нажата кнопка 1,
    // 0x02 – нажата кнопка 2,
    // 0xFE – выход из сервисного режима
    // 0xFF – вход в сервисный режим
    uint8_t isClickedBtn{0};
    // Активность кнопок «старт»
    // (0 – кнопка не активна, 1 – кнопка активна)
    // 0 бит – колонка 1
    // 1 бит – колонка 2
    uint8_t isActiveBtn{0};
    //Внесенная сумма, дискрет – 1 руб.
    uint16_t balance{0};

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
    uint8_t header{0x7F};  // 0x7F
    uint8_t command{0x01}; // 0x01

    //Состояние
    //0x00 – обычное состояние,
    //0x01 – нажата кнопка 1 (если будут кнопки на экране),
    //0x02 – нажата кнопка 2 (если будут кнопки на экране),
    //0x11 – сброс колонки 1,
    //0x12 – сброс колонки 2,
    //0x21 – сервисная кнопка 1 (зарезервировано на будущее)
    //0x22 – сервисная кнопка 2 (зарезервировано на будущее)
    //0x23 – сервисная кнопка 3 (зарезервировано на будущее)
    //0xFF – сброс суточных счетчиков
    enum State : uint8_t
    {
        defaultVal           = 0x00,
        isPressedFirstBtn    = 0x01,
        isPressedSecondBtn   = 0x02,
        resetFirstAzsNode    = 0x11,
        resetSecondAzsNode   = 0x12,
        isPressedServiceBtn1 = 0x21,
        isPressedServiceBtn2 = 0x22,
        isPressedServiceBtn3 = 0x23,
        resetCounters        = 0xFF
    };
    State state{0};

    // 0x01 – Бензин АИ-92
    // 0x02 – Бензин АИ-95
    // 0x03 – Бензин АИ-98
    // 0x04 – ДТ
    // 0x05 – Метан
    // 0x06 – Пропан
    enum GasType : uint8_t
    {
        Gas92   = 0x01,
        Gas95   = 0x02,
        Gas98   = 0x03,
        DT      = 0x04,
        Methane = 0x05,
        Propane = 0x06
    };

    struct AzsNode
    {
        //Тип топлива, колонка
        GasType gasType;
        //Цена, колонка n, дискрет 0,01 руб
        uint16_t price;
    };

    std::array<AzsNode, countAzsNodeMax> azsNodes;

    uint8_t checksum{0};

    void addChecksum()
    {
        constexpr uint16_t size = sizeof(ResponseData);

        checksum     = 0;
        uint8_t* ptr = (uint8_t*)this;
        for (int i = 0; i < size - 1; ++i)
        {
            checksum += ptr[i];
        }
    }

    QByteArray getQByteArray()
    {
        constexpr uint16_t size = sizeof(ResponseData);
        addChecksum();
        QByteArray res;
        uint8_t*   ptr = (uint8_t*)this;
        for (int i = 0; i < size; ++i)
        {
            res.append(ptr[i]);
        }
        return res;
    }
};
#pragma pack(pop)

inline QString getGasTypeString(ResponseData::GasType gasType)
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

inline QString getTextReport(const ReceivedData& info)
{
    QString infoText = QString("Наличн.руб\tобщ-%1\t\tинкас-%2\n"
                               "Безнал.руб\tобщ-%3\t\tинкас-%4\n"
                               "Онлайн.руб\tобщ-%5\t\tинкас-%6\n\n")
                           .arg(info.commonCashSum + info.commonCoinsSum)
                           .arg(info.dailyCashSum + info.dailyCoinsSum)
                           .arg(info.commonCashlessSum)
                           .arg(info.dailyCashlessSum)
                           .arg(info.commonOnlineSum)
                           .arg(info.dailyOnlineSum);

    for (int i = 0; i < countAzsNode; ++i)
    {
        infoText += QString("%1-Литры\t\tобщ-%2\tинкас-%3\n")
                        .arg(i + 1)
                        .arg(static_cast<double>(info.azsNodes[i].common) / 100., 0, 'f', 2)
                        .arg(static_cast<double>(info.azsNodes[i].daily) / 100., 0, 'f', 2);
    }
    return infoText;
}

inline QString getJsonReport(const ReceivedData& info, int countNode)
{
    QJsonObject mainInfo;
    mainInfo.insert("commonCash", static_cast<int>(info.commonCashSum + info.commonCoinsSum));
    mainInfo.insert("dailyCash", static_cast<int>(info.dailyCashSum + info.dailyCoinsSum));

    mainInfo.insert("commonCashless", static_cast<int>(info.commonCashlessSum));
    mainInfo.insert("dailyCashless", static_cast<int>(info.dailyCashlessSum));

    mainInfo.insert("commonOnline", static_cast<int>(info.commonOnlineSum));
    mainInfo.insert("dailyOnline", static_cast<int>(info.dailyOnlineSum));

    mainInfo.insert("countAzsNode", countNode);

    QJsonArray azsNodes;
    for (int i = 0; i < countNode; ++i)
    {
        QJsonObject azsNode;
        azsNode.insert("commonLiters",
                       QString("%1").arg(static_cast<double>(info.azsNodes[i].common) / 100., 0, 'f', 2));
        azsNode.insert("dailyLiters", QString("%1").arg(static_cast<double>(info.azsNodes[i].daily) / 100., 0, 'f', 2));

        azsNode.insert("fuelVolume", QString("%1").arg(info.azsNodes[i].fuelVolume, 0, 'f', 2));
        azsNode.insert("fuelVolumePerc", QString("%1").arg(info.azsNodes[i].fuelVolumePerc, 0, 'f', 2));
        azsNode.insert("density", QString("%1").arg(info.azsNodes[i].density, 0, 'f', 2));
        azsNode.insert("averageTemperature", QString("%1").arg(info.azsNodes[i].averageTemperature, 0, 'f', 2));
        azsNodes.append(azsNode);
    }
    QJsonObject infoJson;
    infoJson.insert("azsNode", azsNodes);
    infoJson.insert("mainInfo", mainInfo);

    // Convert the JSON object to a string
    QString infoText = QString::fromUtf8(QJsonDocument(infoJson).toJson());
    return infoText;
}
