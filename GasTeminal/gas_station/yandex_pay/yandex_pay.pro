QT += core network serialport

CONFIG += c++17 console
QMAKE_CXXFLAGS += -std=c++17 -Wall

SOURCES_PATH = $$PWD/../src
UTILITIES_PATH = $$PWD/../common_utilities

INCLUDEPATH += $${SOURCES_PATH}/
INCLUDEPATH += $${UTILITIES_PATH}/
DEPENDPATH += $${SOURCES_PATH}/
DEPENDPATH += $${UTILITIES_PATH}/

SOURCES += \
        $${SOURCES_PATH}/port.cpp \
        $${SOURCES_PATH}/utilities.cpp \
        $${SOURCES_PATH}/filesystemutilities.cpp \
        $${SOURCES_PATH}/httprequest.cpp \
        comportcontroller.cpp \
        configure.cpp \
        dataprotocol.cpp \
        logging.cpp \
        main.cpp \
        webservercontroller.cpp \
        yandexpayhandler.cpp

HEADERS += \
        $${SOURCES_PATH}/port.h \
        $${SOURCES_PATH}/logging.h \
        $${SOURCES_PATH}/logger_defs.h \
        $${SOURCES_PATH}/utilities.h \
        $${SOURCES_PATH}/filesystemutilities.h \
        $${SOURCES_PATH}/httprequest.h \
        comportcontroller.h \
        configure.h \
        dataprotocol.h \
        dataprotocolcomport.h \
        webservercontroller.h \
        yandexpayhandler.h

target.path = /opt/GasStationPro/$${TARGET}/bin
INSTALLS += target
