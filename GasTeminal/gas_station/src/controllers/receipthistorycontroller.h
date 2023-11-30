#pragma once

#include <QObject>
#include <memory>

#include "receipthistorywindow.h"

class ReceiptHistoryController : public QObject
{
    Q_OBJECT
public:
    explicit ReceiptHistoryController(QObject* parent = nullptr);
    ~ReceiptHistoryController() override = default;

public slots:
    void showDialog();

private:
    std::unique_ptr<ReceiptHistoryWindow> historyReceiptsDialog{nullptr};
};
