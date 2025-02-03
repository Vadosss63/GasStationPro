VERSION = 1.2.2

QT       += core gui network
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17 -Wall

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src/
DEPENDPATH += $$PWD/src/

INCLUDEPATH += $$PWD/src/widgets/
DEPENDPATH += $$PWD/src/widgets/

INCLUDEPATH += $$PWD/src/widgets/input_widget/
DEPENDPATH += $$PWD/src/widgets/input_widget/

INCLUDEPATH += $$PWD/src/controllers/
DEPENDPATH += $$PWD/src/controllers/

INCLUDEPATH += $$PWD/src/interfaces/
DEPENDPATH += $$PWD/src/interfaces/

INCLUDEPATH += $$PWD/src/dataprotocol/
DEPENDPATH += $$PWD/src/dataprotocol/

SOURCES += \
    src/appsettings.cpp \
    src/azsbutton.cpp \
    src/azsnodesettings.cpp \
    src/azsstatistics.cpp \
    src/azsstatisticshandler.cpp \
    src/configure.cpp \
    src/controllers/comportcontroller.cpp \
    src/controllers/mainwindowcontroller.cpp \
    src/controllers/receipthistorycontroller.cpp \
    src/controllers/servicemenucontroller.cpp \
    src/controllers/webservercontroller.cpp \
    src/dataprotocol/fuelutility.cpp \
    src/dataprotocol/volumeconvertor.cpp \
    src/filesystemutilities.cpp \
    src/httprequest.cpp \
    src/ipstorage.cpp \
    src/logger.cpp \
    src/logging.cpp \
    src/main.cpp \
    src/port.cpp \
    src/dataprotocol/priceconvertor.cpp \
    src/receipt.cpp \
    src/report.cpp \
    src/tazs.cpp \
    src/utilities.cpp \
    src/widgets/azsbuttonwidget.cpp \
    src/widgets/input_widget/fuelvalueinputwidget.cpp \
    src/widgets/input_widget/gastypeinputwidget.cpp \
    src/widgets/input_widget/inputwidget.cpp \
    src/widgets/mainwindow.cpp \
    src/widgets/input_widget/priceinputwidget.cpp \
    src/widgets/receipthistorywindow.cpp \
    src/widgets/servicemenuwindow.cpp \
    src/widgets/temporarilyunavailablewidget.cpp \
    src/widgets/labelwidget.cpp

HEADERS += \
    src/azsbutton.h \
    src/azsstatistics.h \
    src/azsstatisticshandler.h \
    src/dataprotocol/constants.h \
    src/dataprotocol/fuelutility.h \
    src/dataprotocol/receiveddata.h \
    src/dataprotocol/responsedata.h \
    src/appsettings.h \
    src/azsnodesettings.h \
    src/configure.h \
    src/controllers/comportcontroller.h \
    src/controllers/mainwindowcontroller.h \
    src/controllers/receipthistorycontroller.h \
    src/controllers/servicemenucontroller.h \
    src/controllers/webservercontroller.h \
    src/dataprotocol/volumeconvertor.h \
    src/filesystemutilities.h \
    src/httprequest.h \
    src/interfaces/azsbuttonhandler.h \
    src/interfaces/ikeypressevent.h \
    src/ipstorage.h \
    src/logger.h \
    src/logger_defs.h \
    src/logging.h \
    src/port.h \
    src/dataprotocol/priceconvertor.h \
    src/receipt.h \
    src/report.h \
    src/tazs.h \
    src/utilities.h \
    src/widgets/azsbuttonwidget.h \
    src/widgets/input_widget/fuelvalueinputwidget.h \
    src/widgets/input_widget/gastypeinputwidget.h \
    src/widgets/input_widget/inputwidget.h \
    src/widgets/itemdelegate.h \
    src/widgets/mainwindow.h \
    src/widgets/input_widget/priceinputwidget.h \
    src/widgets/receipthistorywindow.h \
    src/widgets/servicemenuwindow.h \
    src/widgets/temporarilyunavailablewidget.h \
    src/widgets/labelwidget.h

# Default rules for deployment.
qnx: target.path = /tmp/GasStationPro/$${TARGET}/bin
else: unix:!android: target.path = /opt/GasStationPro/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
