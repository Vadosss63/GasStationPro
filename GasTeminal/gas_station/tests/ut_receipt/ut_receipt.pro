QT += testlib
QT += core
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

include(../include.pri)

SOURCES +=  \
    tst_receipt.cpp \
    $${SOURCE_PATH}/receipt.cpp \
    $${SOURCE_PATH}/appsettings.cpp \

HEADERS += \
    $${SOURCE_PATH}/receipt.h \
    $${SOURCE_PATH}/filesystemutilities.h
