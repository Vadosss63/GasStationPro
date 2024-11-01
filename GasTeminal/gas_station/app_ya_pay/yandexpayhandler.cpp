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
#include "yandexpayhandler.h"

#include "dataprotocolcomport.h"
#include "logging.h"
#include "utilities.h"

YandexPayHandler::YandexPayHandler(const Configure& conf)
{
    comPortController   = std::make_unique<ComPortController>(conf.comPort, conf.baudRate);
    webServerController = std::make_unique<WebServerController>();
    webServerController->setConfigure(conf);

    connect(comPortController.get(), SIGNAL(readyData()), this, SLOT(handleComPort()));
}
YandexPayHandler::~YandexPayHandler() {}

void YandexPayHandler::handleOrder()
{
    auto req = comPortController->getDataReq<OrderComReq>();

    std::optional<OrderRes> orderOpt = webServerController->getOrderFromServer(req.columnId, req.status);

    if (!orderOpt.has_value())
    {
        comPortController->sendToPort(OrderComResp{
            .columnId = req.columnId, .orderType = OrderComResp::NoOrder, .orderVolume = 0, .priceFuel = 0});
        return;
    }

    OrderRes& order = orderOpt.value();

    if (order.columnId < static_cast<int>(orders.size()))
    {
        orders[order.columnId] = order;
    }

    OrderComResp::OrderType orderType{OrderComResp::NoOrder};

    if (order.orderType == "Money")
    {
        orderType = OrderComResp::Money;
    }
    else if (order.orderType == "Liters")
    {
        orderType = OrderComResp::Liters;
    }

    comPortController->sendToPort(OrderComResp{.columnId    = req.columnId,
                                               .orderType   = orderType,
                                               .orderVolume = order.orderVolume,
                                               .priceFuel   = order.priceFuel});
}

void YandexPayHandler::handleCancel()
{
    auto req = comPortController->getDataReq<CanceledComReq>();

    if (req.columnId >= orders.size())
    {
        LOG_ERROR(QString("Invalid columnId %1 for cancel").arg(req.columnId));
        comPortController->sendToPort(CanceledComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    OrderRes& order = orders[req.columnId];

    if (order.orderType.isEmpty())
    {
        LOG_ERROR(QString("Order is empty for cancel req columnId %1 ").arg(req.columnId));
        comPortController->sendToPort(CanceledComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    if (!webServerController->cancelOrder(req.columnId, order.id, getReason(req.reason)))
    {
        LOG_ERROR(QString("Error update cancel order status"));
        comPortController->sendToPort(CanceledComResp{.columnId = req.columnId, .status = StatusResp::serverError});
        return;
    }
    orders[req.columnId] = {};

    comPortController->sendToPort(CanceledComResp{.columnId = req.columnId, .status = StatusResp::ok});
}

void YandexPayHandler::handleAccept()
{
    auto req = comPortController->getDataReq<AcceptComReq>();

    if (req.columnId >= orders.size())
    {
        LOG_ERROR(QString("Invalid columnId %1 for accept").arg(req.columnId));
        comPortController->sendToPort(AcceptComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    OrderRes& order = orders[req.columnId];

    if (order.orderType.isEmpty())
    {
        LOG_ERROR(QString("Order is empty for accept req columnId %1 ").arg(req.columnId));
        comPortController->sendToPort(AcceptComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    if (!webServerController->acceptOrder(req.columnId, order.id))
    {
        LOG_ERROR(QString("Error update accept order status"));
        comPortController->sendToPort(AcceptComResp{.columnId = req.columnId, .status = StatusResp::serverError});
        return;
    }

    comPortController->sendToPort(AcceptComResp{.columnId = req.columnId, .status = StatusResp::ok});
}

void YandexPayHandler::handleFueling()
{
    auto req = comPortController->getDataReq<FuelingComReq>();

    if (req.columnId >= orders.size())
    {
        LOG_ERROR(QString("Invalid columnId %1 for accept").arg(req.columnId));
        comPortController->sendToPort(FuelingComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    OrderRes& order = orders[req.columnId];

    if (order.orderType.isEmpty())
    {
        LOG_ERROR(QString("Order is empty for fueling req columnId %1 ").arg(req.columnId));
        comPortController->sendToPort(FuelingComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    if (!webServerController->fuelingOrder(req.columnId, order.id))
    {
        LOG_ERROR(QString("Error update fueling order status"));
        comPortController->sendToPort(FuelingComResp{.columnId = req.columnId, .status = StatusResp::serverError});
        return;
    }

    comPortController->sendToPort(FuelingComResp{.columnId = req.columnId, .status = StatusResp::ok});
}

void YandexPayHandler::handleCompleted()
{
    CompletedComReq req = comPortController->getDataReq<CompletedComReq>();

    if (req.columnId >= orders.size())
    {
        LOG_ERROR(QString("Invalid columnId %1 for completed").arg(req.columnId));
        comPortController->sendToPort(CompletedComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    OrderRes& order = orders[req.columnId];

    if (order.orderType.isEmpty())
    {
        LOG_ERROR(QString("Order is empty for completed req columnId %1 ").arg(req.columnId));
        comPortController->sendToPort(CompletedComResp{.columnId = req.columnId, .status = StatusResp::invalidReq});
        return;
    }

    CompletedReq completedReq{};
    completedReq.columnId        = req.columnId;
    completedReq.orderId         = order.id;
    completedReq.litre           = req.litres;
    completedReq.extendedOrderId = order.id;
    completedReq.extendedDate    = getCurrentTimestampYaPayFormat();

    if (!webServerController->completedOrder(completedReq))
    {
        LOG_ERROR(QString("Error update completed order status"));
        comPortController->sendToPort(CompletedComResp{.columnId = req.columnId, .status = StatusResp::serverError});
        return;
    }

    orders[req.columnId] = {};
    comPortController->sendToPort(CompletedComResp{.columnId = req.columnId, .status = StatusResp::ok});
}

void YandexPayHandler::handleComPort()
{
    uint8_t requestType = comPortController->getRequestType();
    switch (requestType)
    {
        case OrderComReq::cmd:
            handleOrder();
            break;
        case CanceledComReq::cmd:
            handleCancel();
            break;
        case AcceptComReq::cmd:
            handleAccept();
            break;
        case FuelingComReq::cmd:
            handleFueling();
            break;
        case CompletedComReq::cmd:
            handleCompleted();
            break;
    }
}
