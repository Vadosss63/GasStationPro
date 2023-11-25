#ifndef TEMPORARILYUNAVAILABLEWIDGET_H
#define TEMPORARILYUNAVAILABLEWIDGET_H

#include <QWidget>

class TemporarilyUnavailableWidget : public QWidget
{
public:
    TemporarilyUnavailableWidget(const QString& msg, QWidget* parent = nullptr);
};

#endif // TEMPORARILYUNAVAILABLEWIDGET_H
