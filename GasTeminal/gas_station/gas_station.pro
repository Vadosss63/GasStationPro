QT       += core gui network
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++1Z
QMAKE_CXXFLAGS += -std=c++1z

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src/widgets/
DEPENDPATH += $$PWD/src/widgets/

INCLUDEPATH += $$PWD/src/
DEPENDPATH += $$PWD/src/

SOURCES += \
    src/appsettings.cpp \
    src/configure.cpp \
    src/filesystemutilities.cpp \
    src/httprequest.cpp \
    src/main.cpp \
    src/port.cpp \
    src/widgets/historyreceiptswindow.cpp \
    src/receipt.cpp \
    src/widgets/mainwindow.cpp \
    src/widgets/servicemenuwindow.cpp \
    src/widgets/temporarilyunavailablewidget.cpp \
    src/widgets/labelwidget.cpp

HEADERS += \
    src/appsettings.h \
    src/configure.h \
    src/dataprotocol.h \
    src/filesystemutilities.h \
    src/httprequest.h \
    src/port.h \
    src/receipt.h \
    src/widgets/historyreceiptswindow.h \
    src/widgets/mainwindow.h \
    src/widgets/servicemenuwindow.h \
    src/widgets/temporarilyunavailablewidget.h \
    src/widgets/labelwidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
