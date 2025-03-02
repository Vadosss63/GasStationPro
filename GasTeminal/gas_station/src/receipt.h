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
#include <QStringList>
#include <optional>

struct Receipt
{
    int     time;
    QString date;
    int     numOfAzsNode;
    QString gasType;
    double  countLitres;
    double  cash;
    double  cashless;
    double  online;
    double  sum;

    QString getReceipt() const;
    QString getReceiptJson() const;
};

void                   writeReceiptToFile(const Receipt& receipt);
std::optional<Receipt> readReceiptFromFile(const QString& fileReceiptPath);
QStringList            getListReceiptFiles();
