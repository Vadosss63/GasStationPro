TEMPLATE = app
TARGET = ut_logger

include(../include.pri)

SOURCES +=  \
    ../main.cpp \
    $${SOURCE_PATH}/logger.cpp \
    ut_logger.cpp

HEADERS += \
    $${SOURCE_PATH}/logger.h \
