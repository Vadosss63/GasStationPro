SOURCE_PATH = $$PWD/../src/
STUBS_PATH = $$PWD/stubs/

INCLUDEPATH += $${SOURCE_PATH}
DEPENDPATH += $${SOURCE_PATH}

QMAKE_CXXFLAGS += -Wall -std=c++17
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt
CONFIG += link_pkgconfig
PKGCONFIG += cpputest
