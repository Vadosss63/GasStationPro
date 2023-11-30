#pragma once

#include <QObject>

#include <servicemenuwindow.h>

class ServiceMenuController : public QObject
{
    Q_OBJECT
public:
    ServiceMenuController(QObject* parent = nullptr);

    void createWindow(int showSecondPrice, uint8_t countAzsNode);

    void setupReportTable(const ReceivedData& info);

    std::array<ResponseData::AzsNode, countAzsNodeMax> getAzsNodes() const;

    void showServiceMenu(const ReceivedData& info, const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);
    void closeServiceMenu();

public slots:

    void setupPrice();

signals:
    void setPrice();
    void readCounters();
    void resetCounters();
    void showStatistics();

private:
    void setAzsNodes(const std::array<ResponseData::AzsNode, countAzsNodeMax>& azsNodes);

    void setPriceCashlessToAzsNodes(size_t nodeId);
    void setPriceCashToAzsNodes(size_t nodeId);
    void setGasTypeToAzsNode(size_t nodeId);

    void setPriceCashToServiceMenu(size_t nodeId, uint16_t price);
    void setPriceCashlessToServiceMenu(size_t nodeId, uint16_t price);
    void setGasTypeToServiceMenu(size_t nodeId, ResponseData::GasType gasType);

    std::unique_ptr<ServiceMenuWindow> serviceMenuWindow{nullptr};

    std::array<ResponseData::AzsNode, countAzsNodeMax> azsNodes;

    int     showSecondPrice{false};
    uint8_t countAzsNode{0};
};
