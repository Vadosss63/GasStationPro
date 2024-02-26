#include "webservercontroller.h"

#include "appsettings.h"
#include "filesystemutilities.h"
#include "httprequest.h"
#include "logging.h"
#include <assert.h> 

WebServerController::WebServerController(QObject* parent) : QObject(parent) {}

void WebServerController::sendToServer(const QString& statistics)
{
    const AzsButton azsButton = getServerBtn();
    if (azsButton.idAzs && azsButton.button)
    {
        setBtnFromServer(azsButton);
        resetServerBtn();
    }
    sendReport(statistics);
}

void WebServerController::sendReport(const QString& statistics)
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("name", configure.name);
    params.addQueryItem("address", configure.address);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("count_colum", QString("%1").arg((int)(countAzsNode)));
    params.addQueryItem("is_second_price", QString("%1").arg((int)(configure.showSecondPrice)));

    params.addQueryItem("stats", statistics);
    //LOG_INFO(QString("Sending report: %1").arg(params.toString()));
    QString res = sendPost(configure.host + "/azs_stats", params).msg;
    LOG_INFO(res);
}

bool WebServerController::resetServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);

    Answer answer = sendPost(configure.host + "/reset_azs_button", params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
    return answer.isOk;
}

void WebServerController::setBtnFromServer(const AzsButton& azsButton)
{
    assert(azsBtnHandler);
    azsBtnHandler->setBtnFromServer(azsButton);
}

AzsButton WebServerController::getServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    Answer    answer = sendPost(configure.host + "/get_azs_button", params);
    AzsButton azsButton;
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return azsButton;
    }
    azsButton.readAzsButton(answer.msg);

    LOG_INFO(answer.msg);

    return azsButton;
}

bool WebServerController::sendReceipt(const Receipt& receipt) const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("receipt", receipt.getReceiptJson());
    Answer answer = sendPost(configure.host + "/azs_receipt", params);
    LOG_INFO(answer.msg);

    return answer.isOk;
}

void WebServerController::setConfigure(const Configure& newConfigure)
{
    configure    = newConfigure;
    countAzsNode = newConfigure.activeBtn2 ? 2 : 1;
}

void WebServerController::sendReceiptFiles() const
{
    const QString     folderName      = AppSettings::instance().getReceiptFolderName();
    const QStringList listReciptFiles = getListReciptFiles();

    for (const QString& fileName : listReciptFiles)
    {
        const QString fileReceiptPath{folderName + fileName};
        if (!sendReciptFromFile(fileReceiptPath))
        {
            LOG_ERROR(QString("Failed to send %1 receipt from file").arg(fileName));
            return;
        }

        removeFile(fileReceiptPath);
    }
}

bool WebServerController::sendReciptFromFile(const QString& fileReceiptPath) const
{
    std::optional<Receipt> receipt = readReceiptFromFile(fileReceiptPath);

    if (!receipt.has_value())
    {
        removeFile(fileReceiptPath);
        return false;
    }

    return sendReceipt(receipt.value());
}

void WebServerController::setAzsBtnHandler(AzsBtnHandler* newAzsBtnHandler)
{
    azsBtnHandler = newAzsBtnHandler;
}
