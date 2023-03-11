#pragma once

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <array>
#include <stdint.h>

constexpr uint8_t countColumMax = 2;
constexpr uint8_t countColum    = 2;

constexpr char HEADER           = 0x7F;
constexpr char RECEIVE_DATA_CMD = 0x81;

// inline QChar pubChar = QChar(0x20BD);
inline QChar pubChar = QString("Р").at(0);

#pragma pack(push, 1)
struct ReceiveData
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
    uint32_t commonSumCash{0};
    //Купюры, суточный счётчик, дискрет – 1 руб
    uint32_t dailySumCash{0};

    //Монеты, общий счётчик, дискрет – 1 руб
    uint32_t commonSumCoins{0};
    //Монеты, суточный счётчик, дискрет – 1 руб
    uint32_t dailySumCoins{0};

    //Безнал, общий счётчик, дискрет – 1 руб
    uint32_t commonSumCashless{0};
    //Безнал, суточный счётчик, дискрет – 1 руб
    uint32_t dailySumCashless{0};

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
    std::array<AzsNode, countColumMax> columLiters;
    uint8_t                          checksum{0};

    bool getIsActiveBtn(uint8_t indexBtn) const
    {
        if (indexBtn < countColumMax)
        {
            return isActiveBtn & (1 << indexBtn);
        }
        return false;
    }
    static ReceiveData* getReceiveData(QByteArray& data)
    {
        ReceiveData* receiveData{nullptr};

        if (data.size() != sizeof(ReceiveData))
        {
            return receiveData;
        }
        receiveData = reinterpret_cast<ReceiveData*>(data.data());

        return receiveData;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SendData
{
    uint8_t head{0x7F};    // 0x7F
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
    enum StateEnum : uint8_t
    {
        defaultVal    = 0x00,
        isPressedBtn1 = 0x01,
        isPressedBtn2 = 0x02,
        resetAzsNode1 = 0x11,
        resetAzsNode2 = 0x12,
        isPressedServiceBtn1 = 0x21,
        isPressedServiceBtn2 = 0x22,
        isPressedServiceBtn3 = 0x23,
        resetCounters = 0xFF
    };
    StateEnum state{0};

    //Сумма оплаты онлайн, дискрет – 1 руб.
    uint16_t onlineSum{0};

    //Тип топлива, колонка 1
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
    std::array<GasType, countColumMax> gasTypes;

    //Цена, колонка n, дискрет 0,01 руб
    std::array<uint16_t, countColumMax> prices;

    uint8_t checksum{0};

    void addChecksum()
    {
        uint8_t            sum  = 0;
        constexpr uint16_t size = sizeof(SendData);
        uint8_t*           ptr  = (uint8_t*)this;
        for (int i = 0; i < size - 1; ++i)
        {
            sum += ptr[i];
        }
        checksum = sum;
    }

    QByteArray getQByteArray()
    {
        constexpr uint16_t size = sizeof(SendData);
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

inline QString getGasTypeString(SendData::GasType gasType)
{
    QString result;
    switch (gasType)
    {
        case SendData::Gas92:
            result = "АИ-92";
            break;
        case SendData::Gas95:
            result = "АИ-95";
            break;
        case SendData::Gas98:
            result = "АИ-98";
            break;
        case SendData::DT:
            result = "ДТ";
            break;
        case SendData::Methane:
            result = "Метан";
            break;
        case SendData::Propane:
            result = "Пропан";
            break;
    }
    return result;
}

inline QString getTextReport(const ReceiveData& info)
{
    QString infoText = QString("Наличн.руб\tобщ-%1\t\tинкас-%2\n"
                               "Безнал.руб\tобщ-%3\t\tинкас-%4\n"
                               "Онлайн.руб\tобщ-%5\t\tинкас-%6\n\n")
                           .arg(info.commonSumCash + info.commonSumCoins).arg(info.dailySumCash + info.dailySumCoins)
                           .arg(info.commonSumCashless).arg(info.dailySumCashless)
                           .arg(info.commonOnlineSum).arg(info.dailyOnlineSum);

    for (int i = 0; i < countColum; ++i)
    {
        infoText += QString("%1-Литры\t\tобщ-%2\tинкас-%3\n")
                        .arg(i + 1)
                        .arg(static_cast<double>(info.columLiters[i].common) / 100., 0, 'f', 2)
                        .arg(static_cast<double>(info.columLiters[i].daily) / 100., 0, 'f', 2);
    }
    return infoText;
}

inline QString getJsonTextReport(const ReceiveData &info) {
    QJsonObject mainInfo;
    mainInfo.insert("commonSumCash", static_cast<int>(info.commonSumCash + info.commonSumCoins));
    mainInfo.insert("dailySumCash", static_cast<int>(info.dailySumCash + info.dailySumCoins));

    mainInfo.insert("commonSumCashless", static_cast<int>(info.commonSumCashless));
    mainInfo.insert("dailySumCashless", static_cast<int>(info.dailySumCashless));

    mainInfo.insert("commonOnlineSum", static_cast<int>(info.commonOnlineSum));
    mainInfo.insert("dailyOnlineSum", static_cast<int>(info.dailyOnlineSum));

    QJsonArray columns;
    for (int i = 0; i < countColum; ++i) {
        QJsonObject column;
        column.insert("commonLiters", QString("%1").arg(static_cast<double>(info.columLiters[i].common + 5000 * (i+1)) / 100., 0, 'f', 2));
        column.insert("dailyLiters", QString("%1").arg(static_cast<double>(info.columLiters[i].daily + 250 * (i+1)) / 100., 0, 'f', 2));
        column.insert("remainingFuelLiters", QString("%1").arg(1000));
        columns.append(column);
    }
    QJsonObject infoJson;
    infoJson.insert("columns", columns);
    infoJson.insert("info", mainInfo);


    // Convert the JSON object to a string
    QString infoText = QString::fromUtf8(QJsonDocument(infoJson).toJson());
    return infoText;
}
