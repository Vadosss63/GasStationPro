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

#include <QObject>
#include <memory>
#include <vector>

#include "comportcontroller.h"
#include "webservercontroller.h"

class YandexPayHandler : public QObject
{
    Q_OBJECT
public:
    YandexPayHandler(const Configure& conf);
    ~YandexPayHandler();

    void setTestData(const QByteArray& data) { comPortController->setTestData(data); }

public slots:
    void handleComPort();

private:
    void handleOrder();
    void handleCancel();
    void handleAccept();
    void handleFueling();
    void handleCompleted();

    std::unique_ptr<ComPortController>   comPortController{nullptr};
    std::unique_ptr<WebServerController> webServerController{nullptr};

    std::vector<OrderRes> orders{2};
};
