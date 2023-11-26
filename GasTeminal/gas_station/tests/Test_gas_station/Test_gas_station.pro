QT += testlib
QT += gui core network
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

INCLUDEPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../

SOURCES +=  \
    tst_appsettings.cpp \
    ../../appsettings.cpp

HEADERS += \
    ../../appsettings.h \
    ../../filesystemutilities.h
