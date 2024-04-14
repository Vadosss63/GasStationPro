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

#include "receipthistorywindow.h"

class ReceiptHistoryController : public QObject
{
    Q_OBJECT
public:
    explicit ReceiptHistoryController(QObject* parent = nullptr);
    ~ReceiptHistoryController() override = default;

public slots:
    void showDialog();

private:
    std::unique_ptr<ReceiptHistoryWindow> historyReceiptsDialog{nullptr};
};
