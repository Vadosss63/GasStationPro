#pragma once

#include <QObject>

class ComPortController : public QObject
{
    Q_OBJECT
public:
    ComPortController(QObject* parent = nullptr);
};
