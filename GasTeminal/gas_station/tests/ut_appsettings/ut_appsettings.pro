QT += testlib
QT += core
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

include(../include.pri)

SOURCES +=  \
    tst_appsettings.cpp \
    $${SOURCE_PATH}/appsettings.cpp \

HEADERS += \
    appsettings.h \
    $${SOURCE_PATH}/filesystemutilities.h
