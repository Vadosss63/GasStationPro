#pragma once

#include <QWidget>

class InputWidget : public QWidget
{
    Q_OBJECT
public:
    InputWidget(int command, QWidget* parent = nullptr);
    ~InputWidget() override = default;

    virtual int  getValue() const;
    virtual void setValue(int val);

    int getCommand() const;

private:
    int command{0};
};
