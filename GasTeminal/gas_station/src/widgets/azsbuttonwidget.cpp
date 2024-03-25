#include "azsbuttonwidget.h"

#include <QHBoxLayout>
#include <QStandardItemModel>

AzsButtonWidget::AzsButtonWidget(QWidget* parent) : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);

    cmdCB = new QComboBox;
    cmdCB->setModel(new QStandardItemModel);

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
