QT += testlib
QT += core
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

include(../include.pri)

SOURCES +=  \
    tst_stub.cpp

HEADERS += \
#    $${SOURCE_PATH}/<heder.h> \
