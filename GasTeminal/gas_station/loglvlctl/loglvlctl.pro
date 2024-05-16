QT -= gui
QT += network core

CONFIG += c++17 console
QMAKE_CXXFLAGS += -std=c++17 -Wall

SOURCES_PATH = $$PWD/../src

INCLUDEPATH += $${SOURCES_PATH}/
DEPENDPATH += $${SOURCES_PATH}/

SOURCES += \
        loglvlctl.cpp \
        $${SOURCES_PATH}/ipstorage.cpp \

HEADERS += \
    $${SOURCES_PATH}/ipstorage.h \
    $${SOURCES_PATH}/logger_defs.h \

target.path = /usr/bin
INSTALLS += target
