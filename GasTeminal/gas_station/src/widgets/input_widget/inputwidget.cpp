#include "inputwidget.h"

InputWidget::InputWidget(int command, QWidget* parent) : QWidget(parent), command(command) {}

int InputWidget::getCommand() const
{
    return command;
}

int InputWidget::getValue() const
{
    return 0;
}

void InputWidget::setValue(int) {}
