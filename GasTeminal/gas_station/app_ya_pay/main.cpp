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
#include <QCoreApplication>

#include "configure.h"
#include "logging.h"
#include "yandexpayhandler.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    constexpr auto filePath = "ya_pay_settings.json";

    std::optional<Configure> conf = readConfigure(filePath);
    if (!conf)
    {
        LOG_ERROR("The ya_pay_settings.json contains invalid fields!");
        return EXIT_FAILURE;
    }

    YandexPayHandler yandexPayHandler(conf.value());

    // uint8_t              colId = 0;
    // Package<OrderComReq> orderComReqPackage{};
    // orderComReqPackage.dataType.columnId = colId;
    // orderComReqPackage.dataType.status   = 0;
    // orderComReqPackage.addChecksum();
    // yandexPayHandler.setTestData(orderComReqPackage.getData());
    // yandexPayHandler.handleComPort();

    // Package<CanceledComReq> canceledComReqPackage{};
    // canceledComReqPackage.dataType.columnId = colId;
    // canceledComReqPackage.dataType.reason   = CanceledComReq::ReasonType::invalidPrice;
    // canceledComReqPackage.addChecksum();
    // yandexPayHandler.setTestData(canceledComReqPackage.getData());
    // yandexPayHandler.handleComPort();

    // Package<AcceptComReq> acceptComReqPackage{};
    // acceptComReqPackage.dataType.columnId = colId;
    // acceptComReqPackage.addChecksum();
    // yandexPayHandler.setTestData(acceptComReqPackage.getData());
    // yandexPayHandler.handleComPort();

    // Package<FuelingComReq> fuelingComReqPackage{};
    // fuelingComReqPackage.dataType.columnId = colId;
    // fuelingComReqPackage.addChecksum();
    // yandexPayHandler.setTestData(fuelingComReqPackage.getData());
    // yandexPayHandler.handleComPort();

    // Package<CompletedComReq> completedComReqPackage{};
    // completedComReqPackage.dataType.columnId = colId;
    // completedComReqPackage.dataType.litres   = 1.54;
    // completedComReqPackage.addChecksum();
    // yandexPayHandler.setTestData(completedComReqPackage.getData());
    // yandexPayHandler.handleComPort();

    // return 0;
    return a.exec();
}
