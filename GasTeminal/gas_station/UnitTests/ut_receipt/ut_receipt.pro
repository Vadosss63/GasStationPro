TEMPLATE = app
TARGET = ut_receipt

include(../include.pri)

SOURCES +=  \
    ../main.cpp \
    $${STUBS_PATH}/logging_stub.cpp \
    $${SOURCE_PATH}/receipt.cpp \
    $${SOURCE_PATH}/appsettings.cpp \
    ut_receipt.cpp

HEADERS += \
    $${SOURCE_PATH}/receipt.h \
    $${SOURCE_PATH}/filesystemutilities.h
