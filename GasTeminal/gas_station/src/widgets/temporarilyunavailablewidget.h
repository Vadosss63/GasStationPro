#pragma once

#include <QWidget>

class TemporarilyUnavailableWidget : public QWidget
{
public:
    TemporarilyUnavailableWidget(const QString& msg, QWidget* parent = nullptr);
};
