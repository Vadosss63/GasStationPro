#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <array>
#include <stdint.h>

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

            jsonAzsNode.insert("typeFuel", convertGasTypeToString(azsNode[i].gasType));
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
    enum State : uint8_t
    {
        defaultVal           = 0x00,
        isPressedFirstBtn    = 0x01,
        isPressedSecondBtn   = 0x02,
        blockAzsNode         = 0x11,
        unblockAzsNode       = 0x12,
        isPressedServiceBtn1 = 0x21,
        isPressedServiceBtn2 = 0x22,
        isPressedServiceBtn3 = 0x23,
        resetCounters        = 0xFF
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

    struct AzsNode
    {
        //Тип топлива, колонка
        GasType gasType;
        //Дискрет 0,01 руб
        uint16_t priceCash;
        //Дискрет 0,01 руб
        uint16_t priceCashless;
    };

    using AzsNodes = std::array<AzsNode, countAzsNodeMax>;

    uint8_t  header{0x7F};
    uint8_t  command{0x01};
    State    state{0};
    AzsNodes azsNodes;
    uint8_t  checksum{0};

    QByteArray getQByteArray()
    {
        constexpr uint16_t size = sizeof(ResponseData);
        addChecksum();
        char*      ptr = reinterpret_cast<char*>(this);
        QByteArray res(ptr, size);

        return res;
    }

private:
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
};
#pragma pack(pop)

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
    enum PriceState : uint8_t
    {
        updatePriceCashForFirstNode   = 0x01,
        updatePriceCashForSecondNode  = 0x02,
        updatePriceCashlessFirstNode  = 0x03,
        updatePriceCashlessSecondNode = 0x04,

    };

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
