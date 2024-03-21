VERSION = 0.1.0

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

INCLUDEPATH += $$PWD/src/controllers/
DEPENDPATH += $$PWD/src/controllers/

INCLUDEPATH += $$PWD/src/interfaces/
DEPENDPATH += $$PWD/src/interfaces/

SOURCES += \
    src/appsettings.cpp \
    src/azsnodesettings.cpp \
    src/configure.cpp \
    src/controllers/comportcontroller.cpp \
    src/controllers/mainwindowcontroller.cpp \
    src/controllers/receipthistorycontroller.cpp \
    src/controllers/servicemenucontroller.cpp \
    src/controllers/webservercontroller.cpp \
    src/filesystemutilities.cpp \
    src/httprequest.cpp \
    src/logger.cpp \
    src/logging.cpp \
    src/main.cpp \
    src/port.cpp \
    src/price.cpp \
    src/receipt.cpp \
    src/report.cpp \
    src/tazs.cpp \
    src/utilities.cpp \
    src/widgets/mainwindow.cpp \
    src/widgets/receipthistorywindow.cpp \
    src/widgets/servicemenuwindow.cpp \
    src/widgets/temporarilyunavailablewidget.cpp \
    src/widgets/labelwidget.cpp

HEADERS += \
    src/interfaces/AzsButtonHandler.h \
    src/interfaces/IKeyPressEvent.h \
    src/appsettings.h \
    src/azsnodesettings.h \
    src/configure.h \
    src/controllers/comportcontroller.h \
    src/controllers/mainwindowcontroller.h \
    src/controllers/receipthistorycontroller.h \
    src/controllers/servicemenucontroller.h \
    src/controllers/webservercontroller.h \
    src/dataprotocol.h \
    src/filesystemutilities.h \
    src/httprequest.h \
    src/logger.h \
    src/logger_defs.h \
    src/logging.h \
    src/port.h \
    src/price.h \
    src/receipt.h \
    src/report.h \
    src/tazs.h \
    src/utilities.h \
    src/widgets/mainwindow.h \
    src/widgets/receipthistorywindow.h \
    src/widgets/servicemenuwindow.h \
    src/widgets/temporarilyunavailablewidget.h \
    src/widgets/labelwidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
