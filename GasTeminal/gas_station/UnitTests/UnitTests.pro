TEMPLATE = app
TARGET = LoggerTest

CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt
CONFIG += link_pkgconfig
PKGCONFIG += cpputest

include(include.pri)

SOURCES +=  \
    main.cpp \
    $${SOURCE_PATH}/logger.cpp \
    ut_logger.cpp

HEADERS += \
    $${SOURCE_PATH}/logger.h \
