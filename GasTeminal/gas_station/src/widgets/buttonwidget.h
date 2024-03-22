#pragma once

#include <QWidget>

class ButtonWidget : public QWidget
{
    Q_OBJECT
public:
    ButtonWidget(int command, QWidget* parent = nullptr);
    ~ButtonWidget() override = default;

    virtual int  getValue() const;
    virtual void setValue(int val);

    int getCommand() const;

private:
    int command{0};
};
