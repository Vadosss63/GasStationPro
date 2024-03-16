#include "temporarilyunavailablewidget.h"

#include <QApplication>
#include <QLabel>
#include <QScreen>
#include <QVBoxLayout>

TemporarilyUnavailableWidget::TemporarilyUnavailableWidget(const QString& msg, QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel*      label  = new QLabel(msg, this);
    label->setStyleSheet("color: white; font-size: 60px;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    layout->setAlignment(Qt::AlignCenter);

    QRect screenRect(0, 0, 1280, 1024);

    setGeometry(screenRect);
}
