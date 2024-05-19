QT -= gui
QT += network core

CONFIG += c++17 console
QMAKE_CXXFLAGS += -std=c++17 -Wall

SOURCES_PATH = $$PWD/../src
UTILITIES_PATH = $$PWD/../common_utilities

INCLUDEPATH += $${SOURCES_PATH}/
INCLUDEPATH += $${UTILITIES_PATH}/
DEPENDPATH += $${SOURCES_PATH}/
DEPENDPATH += $${UTILITIES_PATH}/

SOURCES += \
        appupdater.cpp \
        main.cpp \
        $${SOURCES_PATH}/configure.cpp \
        $${SOURCES_PATH}/filesystemutilities.cpp \
        $${SOURCES_PATH}/httprequest.cpp \
        $${SOURCES_PATH}/utilities.cpp \
        $${UTILITIES_PATH}/executor.cpp \
        logging.cpp \
        webservercontroller.cpp \
        workdirectory.cpp

HEADERS += \
    appupdater.h \
    logcommand.h \
    $${SOURCES_PATH}/configure.h \
    $${SOURCES_PATH}/filesystemutilities.h \
    $${SOURCES_PATH}/logger_defs.h \
    $${SOURCES_PATH}/logging.h \
    $${SOURCES_PATH}/httprequest.h \
    $${SOURCES_PATH}/utilities.h \
    $${UTILITIES_PATH}/executor.h \
    webservercontroller.h \
    workdirectory.h

target.path = /opt/GasStationPro/$${TARGET}/bin
INSTALLS += target
