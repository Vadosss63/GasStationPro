#pragma once

#include <QLabel>
#include <QListWidget>
#include <QWidget>

class ReceiptHistoryWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ReceiptHistoryWindow(QWidget* parent = nullptr);
    ~ReceiptHistoryWindow() override = default;

    void settupReceiptList(const QStringList& receipts);
    void setInputSum(int sum);

private:
    void createWidget();

    QLabel*      inputSum    = nullptr;
    QListWidget* receiptList = nullptr;
    const char*  sumTemplate = "Сумма = %1 Р";
};
