TEMPLATE = app
TARGET = ut_configure

include(../include.pri)

SOURCES +=  \
    ../main.cpp \
    $${STUBS_PATH}/logging_stub.cpp \
    $${SOURCE_PATH}/configure.cpp \
    ut_configure.cpp

HEADERS += \
    $${SOURCE_PATH}/configure.h \
    $${SOURCE_PATH}/filesystemutilities.h
