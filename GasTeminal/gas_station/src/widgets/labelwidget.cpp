#include "labelwidget.h"

LabelWidget::LabelWidget(QWidget* parent) : QLabel(parent) {}

void LabelWidget::mousePressEvent(QMouseEvent*)
{
    emit clicked();
}
