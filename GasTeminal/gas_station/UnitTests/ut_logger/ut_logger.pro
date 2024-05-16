TEMPLATE = app
TARGET = ut_logger

include(../include.pri)

QT       += core network
QT       += serialport

SOURCES +=  \
    ../main.cpp \
    $${SOURCE_PATH}/logger.cpp \
    ut_logger.cpp

HEADERS += \
    $${SOURCE_PATH}/logger.h \
    $${SOURCE_PATH}/filesystemutilities.h \
    $${SOURCE_PATH}/ipstorage.h
