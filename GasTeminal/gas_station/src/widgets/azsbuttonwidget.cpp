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

#include "azsbuttonwidget.h"

#include <QHBoxLayout>
#include <QStandardItemModel>

#include "itemdelegate.h"

AzsButtonWidget::AzsButtonWidget(QWidget* parent) : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);

    cmdCB = new QComboBox;
    cmdCB->setModel(new QStandardItemModel);
    ItemDelegate* delegate = new ItemDelegate(cmdCB);
    cmdCB->setItemDelegate(delegate);

    stackedWidgets = new QStackedWidget;

    layout->addWidget(cmdCB);
    layout->addWidget(stackedWidgets);

    connect(cmdCB, QOverload<int>::of(&QComboBox::activated), this, &AzsButtonWidget::switchWidget);
}

void AzsButtonWidget::addItem(const QString& text, QWidget* widget)
{
    cmdCB->addItem(text, QVariant::fromValue(stackedWidgets->count()));
    stackedWidgets->addWidget(widget);
}

QWidget* AzsButtonWidget::getCurrentWidget() const
{
    return stackedWidgets->currentWidget();
}

void AzsButtonWidget::switchWidget(int index)
{
    stackedWidgets->setCurrentIndex(index);
    emit widgetChanged(stackedWidgets->currentWidget());
}
