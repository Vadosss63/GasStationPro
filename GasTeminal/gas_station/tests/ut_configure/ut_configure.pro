QT += testlib
QT += core
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

include(../include.pri)

SOURCES +=  \
    tst_configure.cpp \
    $${SOURCE_PATH}/configure.cpp \

HEADERS += \
    $${SOURCE_PATH}/configure.h \
    $${SOURCE_PATH}/filesystemutilities.h
