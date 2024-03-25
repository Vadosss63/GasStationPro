#pragma once

#include <QObject>

#include "azsbuttonhandler.h"
#include "configure.h"
#include "receipt.h"

class WebServerController : public QObject
{
    Q_OBJECT
public:
    WebServerController(QObject* parent = nullptr);

    void setConfigure(const Configure& newConfigure);

    void sendToServer(const QString& statistics);
    void sendReceiptFiles() const;
    bool sendReceipt(const Receipt& receipt) const;

    void setBtnFromServer(const AzsButton& azsButton);

    void setAzsBtnHandler(AzsBtnHandler* newAzsBtnHandler);

private:
    void sendReport(const QString& statistics);

    AzsButton getServerBtn() const;

    bool resetServerBtn() const;

    bool sendReciptFromFile(const QString& fileReceipt) const;

    Configure configure{};
    uint8_t   countAzsNode{2};

    AzsBtnHandler* azsBtnHandler{nullptr};
};
