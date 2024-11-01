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

#include "webservercontroller.h"

#include <assert.h>

#include "httprequest.h"
#include "logging.h"

WebServerController::WebServerController(QObject* parent) : QObject(parent) {}

std::optional<OrderRes> WebServerController::getOrderFromServer(int columnId, int status) const
{
    OrderReq orderReq{.id = getAzsId(), .token = configure.token, .columnId = columnId, .status = status};

    QByteArray orderReqJson = orderReqToJson(orderReq);

    Answer answer = sendPostJson(configure.host + orderUrl, orderReqJson);

    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return {};
    }

    if (answer.msg.isEmpty())
    {
        return {};
    }

    MsgResp msgResp = getMsgResp(answer.msg.toUtf8());

    if (msgResp.status != "Success" || msgResp.msg.isEmpty())
    {
        return {};
    }

    OrderRes order = parseJsonToOrderRes(msgResp.msg.toUtf8());
    return order;
}

bool WebServerController::cancelOrder(int columnId, const QString& orderId, const QString& reason)
{
    CanceledReq canceledReq{
        .id = getAzsId(), .token = configure.token, .columnId = columnId, .orderId = orderId, .reason = reason};

    QByteArray canceledReqJson = canceledReqToJson(canceledReq);

    Answer answer = sendPostJson(configure.host + cancelOrderUrl, canceledReqJson);

    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return false;
    }

    return true;
}

bool WebServerController::acceptOrder(int columnId, const QString& orderId)
{
    AcceptReq acceptReq{.id = getAzsId(), .token = configure.token, .columnId = columnId, .orderId = orderId};

    QByteArray acceptReqJson = acceptReqToJson(acceptReq);

    Answer answer = sendPostJson(configure.host + acceptOrderUrl, acceptReqJson);

    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return false;
    }

    return true;
}

bool WebServerController::fuelingOrder(int columnId, const QString& orderId)
{
    FuelingReq fuelingReq{.id = getAzsId(), .token = configure.token, .columnId = columnId, .orderId = orderId};

    QByteArray fuelingReqJson = fuelingReqToJson(fuelingReq);

    Answer answer = sendPostJson(configure.host + fuelingOrderUrl, fuelingReqJson);

    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return false;
    }

    return true;
}

bool WebServerController::completedOrder(CompletedReq completedReq)
{
    completedReq.id    = getAzsId();
    completedReq.token = configure.token;

    QByteArray completedReqJson = completedReqToJson(completedReq);

    Answer answer = sendPostJson(configure.host + completedOrderUrl, completedReqJson);

    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return false;
    }

    return true;
}

int WebServerController::getAzsId() const
{
    return QString(configure.id).toInt();
}

void WebServerController::setConfigure(const Configure& newConfigure)
{
    configure = newConfigure;
}
