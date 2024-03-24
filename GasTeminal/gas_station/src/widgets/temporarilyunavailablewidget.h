#pragma once

#include <QWidget>

#include "IKeyPressEvent.h"

class TemporarilyUnavailableWidget : public QWidget
{
public:
    TemporarilyUnavailableWidget(const QString& msg, QWidget* parent = nullptr);
};
