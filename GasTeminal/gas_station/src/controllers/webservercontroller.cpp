/*
 * This file is part of the GasStationPro project.
 *
 * Copyright (C) 2024 Vadim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "webservercontroller.h"

#include <assert.h>

#include "appsettings.h"
#include "azsbutton.h"
#include "filesystemutilities.h"
#include "httprequest.h"
#include "logging.h"

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
    Answer answer = sendPost(configure.host + "/azs_stats", params);
    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
    }
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
    azsBtnHandler->handleAzsBtn(azsButton);
}

AzsButton WebServerController::getServerBtn() const
{
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    Answer answer = sendPost(configure.host + "/get_azs_button", params);

    if (!answer.isOk)
    {
        LOG_ERROR(answer.msg);
        return {};
    }

    std::optional<AzsButton> azsButton = readAzsButton(answer.msg);

    if (!azsButton)
    {
        LOG_ERROR("Error porsing azs button: " + answer.msg);
        return {};
    }

    return *azsButton;
}

bool WebServerController::sendReceipt(const Receipt& receipt) const
{
    constexpr auto apiPath = "/azs_receipt";
    QUrlQuery      params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("receipt", receipt.getReceiptJson());
    Answer answer = sendPost(configure.host + apiPath, params);
    LOG_INFO(answer.msg);

    return answer.isOk;
}

bool WebServerController::sendAzsStatistics(AzsStatistics& statistics) const
{
    constexpr auto apiPath = "/azs/statistics/add";
    statistics.id          = configure.id.toInt();
    QUrlQuery params;
    params.addQueryItem("id", configure.id);
    params.addQueryItem("token", configure.token);
    params.addQueryItem("statistics", statistics.getStatisticsJson());
    Answer answer = sendPost(configure.host + apiPath, params);
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
    const QString     folderName       = AppSettings::instance().getReceiptFolderName();
    const QStringList listReceiptFiles = getListReceiptFiles();

    for (const QString& fileName : listReceiptFiles)
    {
        const QString fileReceiptPath{folderName + fileName};
        if (!sendReceiptFromFile(fileReceiptPath))
        {
            LOG_ERROR(QString("Failed to send %1 receipt from file").arg(fileName));
            return;
        }

        removeFile(fileReceiptPath);
    }
}

bool WebServerController::sendReceiptFromFile(const QString& fileReceiptPath) const
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
