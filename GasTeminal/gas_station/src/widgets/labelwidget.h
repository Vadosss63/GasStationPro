#pragma once

#include <QLabel>

class LabelWidget : public QLabel
{
    Q_OBJECT
public:
    explicit LabelWidget(QWidget* parent = nullptr);
    ~LabelWidget() override = default;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent*) override;
};
