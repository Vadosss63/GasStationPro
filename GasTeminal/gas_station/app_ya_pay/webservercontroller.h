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

#include "configure.h"
#include "dataprotocol.h"

class WebServerController : public QObject
{
    Q_OBJECT
public:
    WebServerController(QObject* parent = nullptr);

    void setConfigure(const Configure& newConfigure);

    std::optional<OrderRes> getOrderFromServer(int columnId, int status) const;

    bool cancelOrder(int columnId, const QString& orderId, const QString& reason);
    bool acceptOrder(int columnId, const QString& orderId);
    bool fuelingOrder(int columnId, const QString& orderId);
    bool completedOrder(CompletedReq completedReq);

private:
    int getAzsId() const;

    Configure configure{};

    static constexpr auto orderUrl          = "/api/azs_order";
    static constexpr auto cancelOrderUrl    = "/api/azs_cancel_order";
    static constexpr auto acceptOrderUrl    = "/api/azs_accept_order";
    static constexpr auto fuelingOrderUrl   = "/api/azs_fueling_order";
    static constexpr auto completedOrderUrl = "/api/azs_completed_order";
};
