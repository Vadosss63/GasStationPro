#pragma once

#include <QObject>
#include <memory>

#include "azsbutton.h"
#include "azsnodesettings.h"
#include "buttonwidget.h"
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
    void setAzsNodes(const AzsNodeSettings& azsNodes);

    std::unique_ptr<ServiceMenuWindow> serviceMenuWindow{nullptr};

    AzsButton azsButton;

    int showSecondPrice{false};

    ButtonWidget* currentButtonWidget{nullptr};

    uint8_t countAzsNode{0};
};
