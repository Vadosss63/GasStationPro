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

#include "temporarilyunavailablewidget.h"

#include <QLabel>
#include <QVBoxLayout>

TemporarilyUnavailableWidget::TemporarilyUnavailableWidget(const QString& msg, QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel*      label  = new QLabel(msg, this);
    label->setStyleSheet("color: white; font-size: 60px;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    layout->setAlignment(Qt::AlignCenter);

    QRect screenRect(0, 0, 1280, 1024);

    setGeometry(screenRect);
}
