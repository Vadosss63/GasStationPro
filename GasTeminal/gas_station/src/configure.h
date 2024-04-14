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
#include <optional>

struct Configure
{
    QString id              = "11111111";
    QString name            = "";
    QString address         = "";
    QString token           = "";
    QString host            = "http://t-azs.ru:8080";
    QString phoneOfSupport  = "8(000)111-66-77";
    int     activeBtn2      = true;
    QString comPort         = "/dev/ttyS4";
    int     baudRate        = 115200;
    int     showSecondPrice = false;
};

std::optional<Configure> readConfigure(const QString& fileName);
