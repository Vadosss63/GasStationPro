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

    template <typename T>
    QString getJsonReport(int countNode, const std::array<T, countAzsNodeMax>& azsNode)
    {
        QJsonObject mainInfo;
        mainInfo.insert("commonCash", static_cast<int>(commonCashSum + commonCoinsSum));
        mainInfo.insert("dailyCash", static_cast<int>(dailyCashSum + dailyCoinsSum));

        mainInfo.insert("commonCashless", static_cast<int>(commonCashlessSum));
        mainInfo.insert("dailyCashless", static_cast<int>(dailyCashlessSum));

        mainInfo.insert("commonOnline", static_cast<int>(commonOnlineSum));
        mainInfo.insert("dailyOnline", static_cast<int>(dailyOnlineSum));

        QJsonArray jsonAzsNodes;
        for (int i = 0; i < countNode; ++i)
        {
            QJsonObject jsonAzsNode;
            jsonAzsNode.insert("commonLiters",
                               QString("%1").arg(static_cast<double>(azsNodes[i].common) / 100., 0, 'f', 2));
            jsonAzsNode.insert("dailyLiters",
                               QString("%1").arg(static_cast<double>(azsNodes[i].daily) / 100., 0, 'f', 2));

            jsonAzsNode.insert("typeFuel", getGasTypeString(azsNode[i].gasType));
            jsonAzsNode.insert("price", static_cast<int>(azsNode[i].priceCash));
            jsonAzsNode.insert("priceCashless", static_cast<int>(azsNode[i].priceCashless));
            jsonAzsNode.insert("fuelVolume", QString("%1").arg(azsNodes[i].fuelVolume, 0, 'f', 2));
            jsonAzsNode.insert("fuelVolumePerc", QString("%1").arg(azsNodes[i].fuelVolumePerc, 0, 'f', 2));
            jsonAzsNode.insert("density", QString("%1").arg(azsNodes[i].density, 0, 'f', 2));
            jsonAzsNode.insert("averageTemperature", QString("%1").arg(azsNodes[i].averageTemperature, 0, 'f', 2));
            jsonAzsNodes.append(jsonAzsNode);
        }
        QJsonObject infoJson;
        infoJson.insert("azs_nodes", jsonAzsNodes);
        infoJson.insert("main_info", mainInfo);

        // Convert the JSON object to a string
        QString infoText = QString::fromUtf8(QJsonDocument(infoJson).toJson());
        return infoText;
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
        //Цена за наличные, колонка n, дискрет 0,01 руб
        uint16_t priceCash;
        //Цена за безналичные, колонка n, дискрет 0,01 руб
        uint16_t priceCashless;
    };

    std::array<AzsNode, countAzsNodeMax> azsNodes;

    uint8_t checksum{0};

    void addChecksum()
    {
        constexpr uint16_t size = sizeof(ResponseData);

        checksum     = 0;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(this);

        for (int i = 0; i < size - 1; ++i)
        {
            checksum += ptr[i];
        }
    }

    QByteArray getQByteArray()
    {
        constexpr uint16_t size = sizeof(ResponseData);
        addChecksum();
        char*      ptr = reinterpret_cast<char*>(this);
        QByteArray res(ptr, size);

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

struct AzsButton
{
    int idAzs{};
    int price{};
    int button{};

    bool readAzsButton(const QString& jsonText)
    {
        const QByteArray jsonData = QByteArray::fromStdString(jsonText.toStdString());
        QJsonDocument    document = QJsonDocument::fromJson(jsonData);

        if (document.isNull())
        {
            qDebug() << "Failed to parse JSON!";
            return false;
        }

        QJsonObject object = document.object();

        if (object.isEmpty() || !object.contains("id_azs") || !object.contains("price") || !object.contains("button"))
        {
            qDebug() << "Missing or invalid field(s)!";
            return false;
        }

        idAzs  = object["id_azs"].toInt();
        price  = object["price"].toInt();
        button = object["button"].toInt();
        return true;
    }
};
