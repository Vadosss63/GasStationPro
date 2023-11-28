#pragma once

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class ReceiptHistoryWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ReceiptHistoryWindow(QWidget* parent = nullptr);
    ~ReceiptHistoryWindow() override {}
    void readReceiptsLogFile();

public slots:
    void showDialog();

private:
    void createWidget();
    void showSum(int sum_);

    QLabel*      sum         = nullptr;
    QListWidget* receiptList = nullptr;
};
