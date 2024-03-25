#pragma once

#include <QHash>
#include <memory>

#include "azsbutton.h"
#include "azsnodesettings.h"
#include "inputwidget.h"
#include "receiveddata.h"
#include "report.h"
#include "servicemenuwindow.h"

class ServiceMenuController : public QObject
{
    Q_OBJECT
public:
    ServiceMenuController(QObject* parent = nullptr);

    void createWindow(int showSecondPrice, uint8_t countAzsNode);

    void setupReportTable(const ReceivedData& info);

    AzsButton getAzsButton() const;

    void showServiceMenu(const AzsReport& azsReport);
    void closeServiceMenu();

    void setCurrentButtonWidget(QWidget* newCurrentButtonWidget);

public slots:

    void pressedButton();

signals:
    void setPrice();
    void readCounters();
    void resetCounters();
    void showStatistics();

private:
    void setupAzsNodeSettingsGetter();
    void setAzsNodes(const AzsNodeSettings& azsNodes);
    void setButtonValue();

    uint8_t countAzsNode{0};
    int     showSecondPrice{false};

    QHash<int, std::function<int()>>   azsNodeSettingsGetter{};
    std::unique_ptr<ServiceMenuWindow> serviceMenuWindow{nullptr};
    AzsNodeSettings                    azsNodeSettings{};
    AzsButton                          azsButton;
    InputWidget*                       currentButtonWidget{nullptr};
};
