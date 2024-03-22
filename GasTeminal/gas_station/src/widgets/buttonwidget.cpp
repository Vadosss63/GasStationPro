#include "buttonwidget.h"

ButtonWidget::ButtonWidget(int command, QWidget* parent) : QWidget(parent), command(command) {}

int ButtonWidget::getCommand() const
{
    return command;
}

int ButtonWidget::getValue() const
{
    return 0;
}

void ButtonWidget::setValue(int) {}
