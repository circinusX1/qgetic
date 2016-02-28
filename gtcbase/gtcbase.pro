#-------------------------------------------------
#
# Project created by QtCreator 2013-10-16T13:29:14
#
#-------------------------------------------------

QT       -= core
DEFINES +=_CFG_GNU_LINUX
DEFINES +=_DEBUG

QMAKE_CXXFLAGS += -std=c++11

TARGET = gtcbase
TEMPLATE = lib
CONFIG += staticlib

SOURCES =   \
    sock.cpp \
    baselib.cpp

HEADERS = \
    sock.h \
    baseutils.h \
    baselib.h \
    basecont.h \
    GNU_LINUX/os.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
