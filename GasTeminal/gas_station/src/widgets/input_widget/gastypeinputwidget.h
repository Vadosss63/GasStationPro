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
#include <QComboBox>

#include "inputwidget.h"

class GasTypeInputWidget : public InputWidget
{
    Q_OBJECT
public:
    explicit GasTypeInputWidget(int command, QWidget* parent = nullptr);
    ~GasTypeInputWidget() override = default;

    int getValue() const override;

    void setValue(int val) override;

private:
    void createWidget();

    QComboBox* gasTypeCBs{nullptr};
};
