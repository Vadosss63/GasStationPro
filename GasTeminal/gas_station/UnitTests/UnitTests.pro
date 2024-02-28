TEMPLATE = app
TARGET = LoggerTest

QMAKE_CXXFLAGS += -Wall -std=c++17
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
