#pragma once

#include <QByteArray>
#include <QString>
#include <array>
#include <stdint.h>

constexpr uint8_t countColumMax = 8;
constexpr uint8_t countColum = 2;

constexpr char HEADER = 0x7F;
constexpr char RECEIVE_DATA_CMD = 0x81;

// inline QChar pubChar = QChar(0x20BD);
inline QChar pubChar = QString("Р").at(0);

#pragma pack(push, 1)
struct ReceiveData {
  uint8_t head{0};    // 0x7F
  uint8_t command{0}; // 0x81
  // 0x00 – обычное состояние,
  // 0x01..0x08 – нажата кнопка 1..8,
  // 0xFE – выход из сервисный режим
  // 0xFF – вход в сервисный режим
  uint8_t isClickedBtn{0};
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
  //Колонка n общий счётчик литров, дискрет – 0,01 л.
  //Колонка n суточный счётчик литров, дискрет – 0,01 л.

  struct Liter {
    uint32_t common{0};
    uint32_t daily{0};
  };
  std::array<Liter, countColumMax> columLiters;
  uint8_t checksum{0};
  bool getIsActiveBtn(uint8_t indexBtn) const {
    if (indexBtn > 7) {
      return false;
    }
    return isActiveBtn & (1 << indexBtn);
  }

  static ReceiveData *getReceiveData(QByteArray &data) {
    ReceiveData *receiveData{nullptr};

    if (data.size() != sizeof(ReceiveData)) {
      return receiveData;
    }
    receiveData = reinterpret_cast<ReceiveData *>(data.data());

    return receiveData;
  }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SendData {
  uint8_t head{0x7F};    // 0x7F
  uint8_t command{0x01}; // 0x01

  //Состояние
  // 0x00 – обычное состояние,
  // 0x01..0x08 – нажата кнопка 1..8 (если будут кнопки на экране),
  // 0xFF – сброс суточных счетчиков
  enum StateEnum : uint8_t {
    defaultVal = 0x00,
    isPressedBtn1 = 0x01,
    isPressedBtn2 = 0x02,
    isPressedBtn3 = 0x03,
    isPressedBtn4 = 0x04,
    isPressedBtn5 = 0x05,
    isPressedBtn6 = 0x06,
    isPressedBtn7 = 0x07,
    isPressedBtn8 = 0x08,
    resetCounters = 0xFF
  };
  StateEnum state{0};
  //Тип топлива, колонка 1
  // 0x01 – Бензин АИ-92
  // 0x02 – Бензин АИ-95
  // 0x03 – Бензин АИ-98
  // 0x04 – ДТ
  // 0x05 – Метан
  // 0x06 – Пропан

  enum GasType : uint8_t {
    Gas92 = 0x01,
    Gas95 = 0x02,
    Gas98 = 0x03,
    DT = 0x04,
    Methane = 0x05,
    Propane = 0x06
  };
  std::array<GasType, countColumMax> gasTypes;

  //Цена, колонка n, дискрет 0,01 руб
  std::array<uint16_t, countColumMax> prices;

  uint8_t checksum{0};

  void addChecksum() {
    uint8_t sum = 0;
    constexpr uint16_t size = sizeof(SendData);
    uint8_t *ptr = (uint8_t *)this;
    for (int i = 0; i < size - 1; ++i) {
      sum += ptr[i];
    }
    checksum = sum;
  }

  QByteArray getQByteArray() {
    constexpr uint16_t size = sizeof(SendData);
    addChecksum();
    QByteArray res;
    uint8_t *ptr = (uint8_t *)this;
    for (int i = 0; i < size; ++i) {
      res.append(ptr[i]);
    }
    return res;
  }
};
#pragma pack(pop)

inline QString getGasTypeString(SendData::GasType gasType) {
  QString result;
  switch (gasType) {
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

inline QString getTextReport(const ReceiveData &info) {
  QString infoText = QString("Нал.руб             общ-%1          инкас-%2")
                         .arg(info.commonSumCash + info.commonSumCoins)
                         .arg(info.dailySumCash + info.dailySumCoins);
  infoText.append("\n");
  infoText += QString("Безнал.руб        общ-%1          инкас-%2")
                  .arg(info.commonSumCashless)
                  .arg(info.dailySumCashless);
  infoText.append("\n");
  for (int i = 0; i < countColum; ++i) {
    infoText += QString("%1-Литры           общ-%2      инкас-%3")
                    .arg(i + 1)
                    .arg(static_cast<double>(info.columLiters[i].common) / 100.,
                         0, 'f', 2)
                    .arg(static_cast<double>(info.columLiters[i].daily) / 100.,
                         0, 'f', 2);
    infoText.append("\n");
  }
  return infoText;
}
