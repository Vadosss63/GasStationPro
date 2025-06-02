import serial
import struct
import time
import argparse

def build_received_data(
    command: int = 0x00,
    is_clicked: int = 0x00,
    is_active: int = 0x00,
    balance_cash: int = 0,
    balance_cashless: int = 0,
    balance_online: int = 0,
    common_cash_sum: int = 0,
    daily_cash_sum: int = 0,
    common_coins_sum: int = 0,
    daily_coins_sum: int = 0,
    common_cashless_sum: int = 0,
    daily_cashless_sum: int = 0,
    common_online_sum: int = 0,
    daily_online_sum: int = 0,
    # Для простоты: две «колонки», заполняем одни и те же поля для обоих узлов
    col1_common: int = 0,
    col1_daily: int = 0,
    col1_volume_m3: float = 0.0,
    col1_volume_perc: float = 0.0,
    col1_density: float = 0.0,
    col1_temp: float = 0.0,
    col2_common: int = 0,
    col2_daily: int = 0,
    col2_volume_m3: float = 0.0,
    col2_volume_perc: float = 0.0,
    col2_density: float = 0.0,
    col2_temp: float = 0.0,
) -> bytes:
    """
    Формирует и возвращает байтовое представление структуры ReceivedData.
    Поля по умолчанию равны нулю, но вы можете передать желаемые значения.
    """
    # 1) Упакуем фиксированные поля:
    # '<'  — little-endian
    # B B B B — head, command, is_clicked, is_active
    # H H H — balance_cash, balance_cashless, balance_online
    header = 0x7F
    fmt_fixed = '<BBBBHHH'
    packed = struct.pack(
        fmt_fixed,
        header,
        command,
        is_clicked,
        is_active,
        balance_cash,
        balance_cashless,
        balance_online
    )

    # 2) Упакуем восемь 32-битных счётчиков (uint32_t)
    fmt_sums = '<' + 'I' * 8
    packed += struct.pack(
        fmt_sums,
        common_cash_sum,
        daily_cash_sum,
        common_coins_sum,
        daily_coins_sum,
        common_cashless_sum,
        daily_cashless_sum,
        common_online_sum,
        daily_online_sum
    )

    # 3) Упакуем данные для колонки 1:
    # I I f f f f  — uint32, uint32, float, float, float, float
    fmt_node = '<IIffff'
    packed += struct.pack(
        fmt_node,
        col1_common,
        col1_daily,
        col1_volume_m3,
        col1_volume_perc,
        col1_density,
        col1_temp
    )

    # 4) Аналогично для колонки 2:
    packed += struct.pack(
        fmt_node,
        col2_common,
        col2_daily,
        col2_volume_m3,
        col2_volume_perc,
        col2_density,
        col2_temp
    )

    # На текущем этапе длина packed = 1+1+1+1 + 2+2+2 + 8*4 + 2*(2*4 + 4*4) = 90 байт.
    # 5) Посчитаем checksum (сумма всех предыдущих байт по модулю 256) и допишем его
    checksum = sum(packed) & 0xFF
    packed += struct.pack('<B', checksum)

    # Теперь len(packed) == 91
    return packed

def parse_response_data(raw: bytes):
    """
    Разбирает 7 байт ResponseData и возвращает словарь с полями.
    Если длина пакета != 7 или checksum неверный, возвращает None.
    """
    if len(raw) != 7:
        return None

    # '<' — little endian, B B I B
    header, command, data_value, recv_checksum = struct.unpack('<BBIB', raw)

    # Проверяем header
    if header != 0x7F:
        print("Invalid header in ResponseData:", header)
        return None

    # Проверяем контрольную сумму
    # Суммируем первые 6 байт (header, command, 4 байта data) & 0xFF
    calc_checksum = (header + command + 
                     (data_value & 0xFF) +
                     ((data_value >> 8) & 0xFF) +
                     ((data_value >> 16) & 0xFF) +
                     ((data_value >> 24) & 0xFF)
                    ) & 0xFF
    if calc_checksum != recv_checksum:
        print(f"Checksum mismatch in ResponseData: calc=0x{calc_checksum:02X}, recv=0x{recv_checksum:02X}")
        return None

    return {
        'command': command,
        'data': data_value
    }

def main():
    parser = argparse.ArgumentParser(description="Имитация отправки ReceivedData и чтения ResponseData")
    parser.add_argument('port', help="Имя COM-порта (например, COM3 или /dev/ttyUSB0)")
    parser.add_argument('--baudrate', type=int, default=9600, help="Скорость порта (по умолчанию 9600)")
    parser.add_argument('--interval', type=float, default=10.0, help="Интервал отправки, сек (по умолчанию 10.0)")
    args = parser.parse_args()

    try:
        ser = serial.Serial(port=args.port, baudrate=args.baudrate, timeout=0.5)
    except serial.SerialException as e:
        print("Не удалось открыть порт:", e)
        return

    print(f"Открыт порт {args.port} @ {args.baudrate} бод. Нажмите Ctrl+C, чтобы выйти.")
    
    cmd = 0x00 

    try:
        while True:
            # ====== 1. Сформируем пакет ReceivedData ======
            # Здесь можно динамически менять поля, например счётчики или балансы
            packet = build_received_data(
                command=cmd,           
                is_clicked=0x00,
                is_active=0x01,
                balance_cash=100,       # скажем, внесено 100 руб наличными
                balance_cashless=50,    # и 50 руб безналом
                balance_online=0,       # онлайн не использовано
                common_cash_sum=5000,   # общий счётчик купюр = 5000 руб
                daily_cash_sum=200,     # суточный счётчик купюр = 200 руб
                common_coins_sum=3000,
                daily_coins_sum=150,
                common_cashless_sum=8000,
                daily_cashless_sum=400,
                common_online_sum=0,
                daily_online_sum=0,
                # заполнение для колонок (узлы AzsNode):
                col1_common=123456,     # всего залито по колонке 1: 1234.56 л (123456 * 0.01)
                col1_daily=7890,        # сегодня по колонке 1: 78.90 л
                col1_volume_m3=12.345,  # объём бака в м³
                col1_volume_perc=50.0,  # 50 %
                col1_density=0.75,      # 0.75 т/м³
                col1_temp=15.5,         # 15.5 °C

                col2_common=654321,     # всего залито по колонке 2: 6543.21 л
                col2_daily=4321,        # сегодня по колонке 2: 43.21 л
                col2_volume_m3=23.456,
                col2_volume_perc=75.0,
                col2_density=0.80,
                col2_temp=18.2
            )
            # Отправляем 91 байт в COM-порт
            ser.write(packet)
            print(f"[{time.strftime('%H:%M:%S')}] Отправлен ReceivedData ({len(packet)} байт)")

            # ====== 2. Ждём и читаем ответ (ResponseData) ======
            # Попробуем прочитать ровно 7 байт
            raw_resp = ser.read(7)
            if len(raw_resp) == 7:
                resp = parse_response_data(raw_resp)
                cmd = resp['command'] if resp else 0x00
                if resp is not None:
                    print(f"  ← ResponseData: command=0x{resp['command']:02X}, data={resp['data']}")
                else:
                    print("  ← Получены 7 байт, но не удалось разобрать ResponseData")
            else:
                # Если пришло меньше данных (timeout) – ничего не выводим
                pass

            # ====== 3. Ждём перед следующей итерацией ======
            time.sleep(args.interval)

    except KeyboardInterrupt:
        print("\nВыход по Ctrl+C.")
    finally:
        ser.close()

if __name__ == '__main__':
    main()
