TEMPLATE = app
TARGET = ut_appsettings

include(../include.pri)

SOURCES +=  \
    ../main.cpp \
    $${STUBS_PATH}/logging_stub.cpp \
    $${SOURCE_PATH}/appsettings.cpp \
    ut_appsettings.cpp

HEADERS += \
    $${SOURCE_PATH}/appsettings.h \
    $${SOURCE_PATH}/filesystemutilities.h
