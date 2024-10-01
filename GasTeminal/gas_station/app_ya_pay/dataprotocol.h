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
#include <QString>

struct OrderReq
{
    int     id;
    QString token;
    int     columnId;
    int     status;
};

struct OrderRes
{
    QString id;
    QString token;
    QString dateCreate;
    QString orderType;
    float   orderVolume;
    int     columnId;
    float   priceFuel;
};

struct CanceledReq
{
    int     id;
    QString token;
    QString orderId;
    QString reason;
};

struct AcceptReq
{
    int     id;
    QString token;
    QString orderId;
};

struct FuelingReq
{
    int     id;
    QString token;
    QString orderId;
};

struct CompletedReq
{
    int     id;
    QString token;
    QString orderId;
    float   litre;
    QString extendedOrderId;
    QString extendedDate;
};

OrderRes   parseJsonToOrderRes(const QByteArray& jsonData);
QByteArray orderReqToJson(const OrderReq& orderReq);
QByteArray canceledReqToJson(const CanceledReq& canceledReq);
QByteArray acceptReqToJson(const AcceptReq& acceptReq);
QByteArray fuelingReqToJson(const FuelingReq& fuelingReq);
QByteArray completedReqToJson(const CompletedReq& completedReq);
