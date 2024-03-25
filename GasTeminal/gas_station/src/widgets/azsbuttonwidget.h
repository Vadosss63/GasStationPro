#pragma once

#include <QComboBox>
#include <QStackedWidget>

class AzsButtonWidget : public QWidget
{
    Q_OBJECT
public:
    AzsButtonWidget(QWidget* parent = nullptr);

    void addItem(const QString& text, QWidget* widget);

    QWidget* getCurrentWidget() const;

signals:
    void widgetChanged(QWidget* newWidget);

private slots:
    void switchWidget(int index);

private:
    QComboBox*      cmdCB{nullptr};
    QStackedWidget* stackedWidgets{nullptr};
};
