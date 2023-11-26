QT += testlib
QT += gui core network
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

INCLUDEPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../

SOURCES +=  \
    tst_configure.cpp \
    ../../configure.cpp \

HEADERS += \
    ../../configure.h \
    ../../filesystemutilities.h
