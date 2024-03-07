QT -= gui
QT += network core

CONFIG += c++17 console
QMAKE_CXXFLAGS += -std=c++17 -Wall

SOURCES_PATH = $$PWD/../src

INCLUDEPATH += $${SOURCES_PATH}/
DEPENDPATH += $${SOURCES_PATH}/

SOURCES += \
        appupdater.cpp \
        main.cpp \
        $${SOURCES_PATH}/configure.cpp \
        $${SOURCES_PATH}/filesystemutilities.cpp \
        $${SOURCES_PATH}/httprequest.cpp \
        $${SOURCES_PATH}/utilities.cpp \
        logging.cpp \
        webservercontroller.cpp

HEADERS += \
    appupdater.h \
    logcommand.h \
    $${SOURCES_PATH}/configure.h \
    $${SOURCES_PATH}/filesystemutilities.h \
    $${SOURCES_PATH}/logger_defs.h \
    $${SOURCES_PATH}/logging.h \
    $${SOURCES_PATH}/httprequest.h \
    $${SOURCES_PATH}/utilities.h \
    webservercontroller.h
