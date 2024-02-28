QT -= gui
QT += network core

CONFIG += c++17 console
QMAKE_CXXFLAGS += -std=c++17

SOURCES_PATH = $$PWD/../src

INCLUDEPATH += $${SOURCES_PATH}/
DEPENDPATH += $${SOURCES_PATH}/

SOURCES += \
        loguploader.cpp \
        main.cpp \
        $${SOURCES_PATH}/configure.cpp \
        $${SOURCES_PATH}/filesystemutilities.cpp \
        $${SOURCES_PATH}/httprequest.cpp \
        logging.cpp \
        webservercontroller.cpp

HEADERS += \
    loguploader.h \
    $${SOURCES_PATH}/configure.h \
    $${SOURCES_PATH}/filesystemutilities.h \
    $${SOURCES_PATH}/logger_defs.h \
    $${SOURCES_PATH}/logging.h \
    $${SOURCES_PATH}/httprequest.h \
    webservercontroller.h
