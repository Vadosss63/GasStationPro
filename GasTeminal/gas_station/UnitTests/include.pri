SOURCE_PATH = $$PWD/../src/
STUBS_PATH = $$PWD/stubs/

INCLUDEPATH += $${SOURCE_PATH}
DEPENDPATH += $${SOURCE_PATH}

QMAKE_CXXFLAGS += -Wall -std=c++20
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt
CONFIG += link_pkgconfig
PKGCONFIG += cpputest
