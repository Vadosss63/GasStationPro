#include "servicemenucontroller.h"

#include "fuelvalueinputwidget.h"
#include "gastypeinputwidget.h"
#include "price.h"
#include "priceinputwidget.h"

namespace
{
template <typename InputWidget>
void addInputWidget(AzsButtonWidget* azsButtonWidget, const QString& text, int val)
{
    InputWidget* price1Input = new InputWidget(val);
    azsButtonWidget->addItem(text, price1Input);
}
}

ServiceMenuController::ServiceMenuController(QObject* parent) : QObject(parent) {}

void ServiceMenuController::createWindow(int showSecondPrice, uint8_t countAzsNode)
{
    serviceMenuWindow = std::make_unique<ServiceMenuWindow>();

    AzsButtonWidget* azsButtonWidget = new AzsButtonWidget;

    if (countAzsNode == 1 && showSecondPrice == false)
    {
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Цена", ResponseData::setPriceCash1);
        addInputWidget<GasTypeInputWidget>(azsButtonWidget, "Топливо", ResponseData::setGasType1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Приход", ResponseData::setFuelArrival1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Блок при", ResponseData::setLockFuelValue1);
    }
    else if (countAzsNode == 2 && showSecondPrice == false)
    {
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Цена-1", ResponseData::setPriceCash1);
        addInputWidget<GasTypeInputWidget>(azsButtonWidget, "Топливо-1", ResponseData::setGasType1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Приход-1", ResponseData::setFuelArrival1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Блок-1 при", ResponseData::setLockFuelValue1);

        addInputWidget<PriceInputWidget>(azsButtonWidget, "Цена-2", ResponseData::setPriceCash2);
        addInputWidget<GasTypeInputWidget>(azsButtonWidget, "Топливо-2", ResponseData::setGasType2);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Приход-2", ResponseData::setFuelArrival2);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Блок-2 при", ResponseData::setLockFuelValue2);
    }
    else if (countAzsNode == 1 && showSecondPrice == true)
    {
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Наличн", ResponseData::setPriceCash1);
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Безнал", ResponseData::setPriceCashless1);
        addInputWidget<GasTypeInputWidget>(azsButtonWidget, "Топливо", ResponseData::setGasType1);

        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Приход", ResponseData::setFuelArrival1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Блок при", ResponseData::setLockFuelValue1);
    }
    else if (countAzsNode == 2 && showSecondPrice == true)
    {
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Наличн-1", ResponseData::setPriceCash1);
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Безнал-1", ResponseData::setPriceCashless1);
        addInputWidget<GasTypeInputWidget>(azsButtonWidget, "Топливо-1", ResponseData::setGasType1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Приход-1", ResponseData::setFuelArrival1);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Блок-1 при", ResponseData::setLockFuelValue1);

        addInputWidget<PriceInputWidget>(azsButtonWidget, "Наличн-2", ResponseData::setPriceCash2);
        addInputWidget<PriceInputWidget>(azsButtonWidget, "Безнал-2", ResponseData::setPriceCashless2);
        addInputWidget<GasTypeInputWidget>(azsButtonWidget, "Топливо-2", ResponseData::setGasType2);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Приход-2", ResponseData::setFuelArrival2);
        addInputWidget<FuelValueInputWidget>(azsButtonWidget, "Блок-2 при", ResponseData::setLockFuelValue2);
    }

    serviceMenuWindow->createNodes(azsButtonWidget, countAzsNode);

    connect(azsButtonWidget, &AzsButtonWidget::widgetChanged, this, &ServiceMenuController::setCurrentButtonWidget);

    setCurrentButtonWidget(azsButtonWidget->getCurrentWidget());

    this->showSecondPrice = showSecondPrice;
    this->countAzsNode    = countAzsNode;

    connect(serviceMenuWindow.get(), SIGNAL(pressedButton()), this, SLOT(pressedButton()));
    connect(serviceMenuWindow.get(), SIGNAL(readCounters()), this, SIGNAL(readCounters()));
    connect(serviceMenuWindow.get(), SIGNAL(resetCounters()), this, SIGNAL(resetCounters()));
    connect(serviceMenuWindow.get(), SIGNAL(showStatistics()), this, SIGNAL(showStatistics()));

    setupAzsNodeSettingsGetter();
}

void ServiceMenuController::setupReportTable(const ReceivedData& info)
{
    serviceMenuWindow->setCommonCash(info.commonCashSum + info.commonCoinsSum);
    serviceMenuWindow->setDailyCash(info.dailyCashSum + info.dailyCoinsSum);

    serviceMenuWindow->setCommonCashless(info.commonCashlessSum);
    serviceMenuWindow->setDailyCashless(info.dailyCashlessSum);

    serviceMenuWindow->setCommonOnline(info.commonOnlineSum);
    serviceMenuWindow->setDailyOnline(info.dailyOnlineSum);
    for (int nodeId = 0; nodeId < countAzsNode; ++nodeId)
    {
        double common = static_cast<double>(info.azsNodes[nodeId].common) / 100.;
        double daily  = static_cast<double>(info.azsNodes[nodeId].daily) / 100.;
        serviceMenuWindow->setLiters(common, daily, nodeId, countAzsNode);
    }
}

void ServiceMenuController::showServiceMenu(const AzsReport& azsReport)
{
    setAzsNodes(azsReport.azsNode);
    setupReportTable(azsReport.rec);
    serviceMenuWindow->show();
    serviceMenuWindow->setFocus();
}

void ServiceMenuController::setAzsNodes(const AzsNodeSettings& azsNodes)
{
    azsNodeSettings = azsNodes;
    setButtonValue();
}

void ServiceMenuController::setButtonValue()
{
    int command = currentButtonWidget->getCommand();
    if (azsNodeSettingsGetter.contains(command))
    {
        currentButtonWidget->setValue(azsNodeSettingsGetter[command]());
    }
}

void ServiceMenuController::setCurrentButtonWidget(QWidget* newCurrentButtonWidget)
{
    if (newCurrentButtonWidget)
    {
        currentButtonWidget = static_cast<ButtonWidget*>(newCurrentButtonWidget);
        setButtonValue();
    }
}

void ServiceMenuController::pressedButton()
{
    azsButton = {-1, currentButtonWidget->getValue(), currentButtonWidget->getCommand()};

    emit setPrice();
    closeServiceMenu();
}

void ServiceMenuController::setupAzsNodeSettingsGetter()
{
    azsNodeSettingsGetter.insert(ResponseData::setPriceCash1,
                                 [&]() -> int { return azsNodeSettings.nodes[0].priceCash; });
    azsNodeSettingsGetter.insert(ResponseData::setPriceCash2,
                                 [&]() -> int { return azsNodeSettings.nodes[1].priceCash; });

    azsNodeSettingsGetter.insert(ResponseData::setPriceCashless1,
                                 [&]() -> int { return azsNodeSettings.nodes[0].priceCashless; });
    azsNodeSettingsGetter.insert(ResponseData::setPriceCashless2,
                                 [&]() -> int { return azsNodeSettings.nodes[1].priceCashless; });

    azsNodeSettingsGetter.insert(ResponseData::setGasType1, [&]() -> int { return azsNodeSettings.nodes[0].gasType; });
    azsNodeSettingsGetter.insert(ResponseData::setGasType1, [&]() -> int { return azsNodeSettings.nodes[1].gasType; });

    azsNodeSettingsGetter.insert(ResponseData::setFuelArrival1,
                                 [&]() -> int { return azsNodeSettings.nodes[0].fuelArrival; });
    azsNodeSettingsGetter.insert(ResponseData::setFuelArrival2,
                                 [&]() -> int { return azsNodeSettings.nodes[1].fuelArrival; });

    azsNodeSettingsGetter.insert(ResponseData::setLockFuelValue1,
                                 [&]() -> int { return azsNodeSettings.nodes[0].lockFuelValue; });
    azsNodeSettingsGetter.insert(ResponseData::setLockFuelValue2,
                                 [&]() -> int { return azsNodeSettings.nodes[1].lockFuelValue; });
}

AzsButton ServiceMenuController::getAzsButton() const
{
    return azsButton;
}

void ServiceMenuController::closeServiceMenu()
{
    serviceMenuWindow->close();
}
