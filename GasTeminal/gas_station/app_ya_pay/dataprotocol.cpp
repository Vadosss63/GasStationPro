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
#include "dataprotocol.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "logging.h"

OrderRes parseJsonToOrderRes(const QByteArray& jsonData)
{
    OrderRes orderRes;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    if (!jsonDoc.isObject())
    {
        LOG_ERROR("Invalid JSON format");
        return orderRes;
    }

    QJsonObject jsonObj = jsonDoc.object();

    orderRes.id          = jsonObj["Id"].toString();
    orderRes.dateCreate  = jsonObj["DateCreate"].toString();
    orderRes.orderType   = jsonObj["OrderType"].toString();
    orderRes.orderVolume = static_cast<float>(jsonObj["OrderVolume"].toDouble());
    orderRes.columnId    = jsonObj["ColumnId"].toInt();
    orderRes.priceFuel   = static_cast<float>(jsonObj["PriceFuel"].toDouble());
    return orderRes;
}

QByteArray getJsonData(const QJsonObject& jsonObj)
{
    QJsonDocument jsonDoc(jsonObj);
    QByteArray    jsonData   = jsonDoc.toJson(QJsonDocument::Compact);
    QString       jsonString = QString::fromUtf8(jsonData);
    jsonData                 = jsonString.toUtf8();
    return jsonData;
}

QByteArray orderReqToJson(const OrderReq& orderReq)
{
    QJsonObject jsonObj;
    jsonObj["Id"]       = orderReq.id;
    jsonObj["Token"]    = orderReq.token;
    jsonObj["ColumnId"] = orderReq.columnId;
    jsonObj["Status"]   = orderReq.status;

    return getJsonData(jsonObj);
}

QByteArray canceledReqToJson(const CanceledReq& canceledReq)
{
    QJsonObject jsonObj;
    jsonObj["Id"]       = canceledReq.id;
    jsonObj["Token"]    = canceledReq.token;
    jsonObj["columnId"] = canceledReq.columnId;
    jsonObj["OrderId"]  = canceledReq.orderId;
    jsonObj["Reason"]   = canceledReq.reason;

    return getJsonData(jsonObj);
}

QByteArray acceptReqToJson(const AcceptReq& acceptReq)
{
    QJsonObject jsonObj;
    jsonObj["Id"]       = acceptReq.id;
    jsonObj["Token"]    = acceptReq.token;
    jsonObj["columnId"] = acceptReq.columnId;
    jsonObj["OrderId"]  = acceptReq.orderId;

    return getJsonData(jsonObj);
}

QByteArray fuelingReqToJson(const FuelingReq& fuelingReq)
{
    QJsonObject jsonObj;
    jsonObj["Id"]       = fuelingReq.id;
    jsonObj["Token"]    = fuelingReq.token;
    jsonObj["columnId"] = fuelingReq.columnId;
    jsonObj["OrderId"]  = fuelingReq.orderId;

    return getJsonData(jsonObj);
}

QByteArray completedReqToJson(const CompletedReq& completedReq)
{
    QJsonObject jsonObj;
    jsonObj["Id"]              = completedReq.id;
    jsonObj["Token"]           = completedReq.token;
    jsonObj["columnId"]        = completedReq.columnId;
    jsonObj["OrderId"]         = completedReq.orderId;
    jsonObj["Litre"]           = completedReq.litre;
    jsonObj["ExtendedOrderId"] = completedReq.extendedOrderId;
    jsonObj["ExtendedDate"]    = completedReq.extendedDate;

    return getJsonData(jsonObj);
}
