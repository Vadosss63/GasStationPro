#pragma once

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class HistoryReceiptsDialog : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryReceiptsDialog(QWidget* parent = nullptr);
    ~HistoryReceiptsDialog() override {}
    void readReceiptsLogFile();

public slots:
    void showDialog();

private:
    void createWidget();
    void showSum(int sum_);

    QLabel*      sum         = nullptr;
    QListWidget* receiptList = nullptr;
};
