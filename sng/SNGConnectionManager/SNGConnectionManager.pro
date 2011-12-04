# -------------------------------------------------
# Project created by QtCreator 2009-11-03T02:57:52
# -------------------------------------------------
VERSION = 1.0.4
DEPENDS =

QT += network \
    xml
QT -= gui

TARGET = SNGConnectionManager
TEMPLATE = lib
DEFINES += SNGCONNECTIONMANAGER_LIBRARY

SOURCES += connectionmanager.cpp \
    connectionframe.cpp \
    objectconnectionsupport.cpp \
    groupaddress.cpp
HEADERS += ../sng_include/global.h \
    connectionmanager.h \
    ../sng_include/connectionframe.h \
    ../sng_include/objectconnectionsupport.h \
    ../sng_include/groupaddress.h

DESTDIR = ./lib
INCLUDEPATH += ./ \
    ../sng_include/


