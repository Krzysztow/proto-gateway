#-------------------------------------------------
#
# Project created by QtCreator 2011-07-19T19:54:05
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = BACnet_KW
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    bacnetvirtuallinklayer.cpp \
    bacnetnetworklayer.cpp \
    bacnetbbmdhandler.cpp \
    bacnetbipaddress.cpp \
    bacnetudptransportlayer.cpp \
    bacnetbiptransportlayer.cpp \
    bacnetaddress.cpp \
    bacnetrouter.cpp \
    bacnetapplicationlayer.cpp \
    bacnetnpci.cpp \
    helpercoder.cpp \
    bacnetbuffermanager.cpp \
    buffer.cpp \
    bacnetpci.cpp \
    bacnetcoder.cpp \
    services/bacnetreadproperty.cpp \
    bacnettagparser.cpp

HEADERS += \
    bacnetvirtuallinklayer.h \
    bacnetnetworklayer.h \
    bacnetbbmdhandler.h \
    bacnetbipaddress.h \
    bacnetudptransportlayer.h \
    bacnetcommon.h \
    bacnettransportlayer.h \
    bacnetbiptransportlayer.h \
    bacnetaddress.h \
    bacnetrouter.h \
    bacnetapplicationlayer.h \
    bitfields.h \
    bacnetnpci.h \
    helpercoder.h \
    bacnetbuffermanager.h \
    buffer.h \
    bacnetpci.h \
    bacnetcoder.h \
    services/bacnetreadproperty.h \
    bacnettagparser.h
