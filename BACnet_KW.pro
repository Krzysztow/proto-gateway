# -------------------------------------------------
# Project created by QtCreator 2011-07-19T19:54:05
# -------------------------------------------------
QT += core \
    network
QT -= gui
TARGET = BACnet_KW
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

INCLUDEPATH += internal \
    external \
    applayer

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
    bacnettagparser.cpp \
    property.cpp \
    asynchowner.cpp \
    bacnettsm2.cpp \
#    bacnetwritepropertyservice.cpp \
    externalobjectshandler.cpp \
    cdm.cpp \
    bacnetwritepropertyservicehandler.cpp \
    bacnetreadpropertyservicehandler.cpp \
    internalobjectshandler.cpp \
    bacnettsm.cpp \
    bacnetreadpropertyack.cpp \ # bacnetreadpropertyservice.cpp \
    bacnetcommon.cpp \
    bacnetobjectinternalsupport.cpp \
    bacnetexternalobjects.cpp \
    analoginputobject.cpp \
    bacnetdeviceobject.cpp \
    bacnetobject.cpp \
    bacnetprimitivedata.cpp \ # bacneterrorack.cpp \
    servicefactory.cpp \
    asynchronousbacnettsmaction.cpp \
    bacnetservice.cpp \
    error.cpp \
    propertyvalue.cpp \
    bacnetdataabstract.cpp \
    bacnetconstructeddata.cpp \
    bacnetdefaultobject.cpp \
    bacnetsubscribecov.cpp \
    bacnetunconfirmedcov.cpp \
    bacnetconfirmedcov.cpp \
    sequenceof.cpp \
    bacnetdata.cpp \
    bacnetserviceack.cpp \
    propertyfactory.cpp \ # check.cpp \
    readpropertyservicedata.cpp \
    bacnetinternaladdresshelper.cpp \
    whoisservicedata.cpp \
    iamservicedata.cpp \
    whohasservicedata.cpp \
    ihaveservicedata.cpp \
    subscribecovservicedata.cpp \
    writepropertyservicedata.cpp \
    covsupport.cpp \
    covnotificationrequestdata.cpp \
    covconfnotificationservicehandler.cpp \
    datavisitor.cpp \
    covincrementhandlers.cpp \
    bacnetcovsubscription.cpp \
    covsubscriptionstimehandler.cpp \
    discoverywrapper.cpp \
    invokeidgenerator.cpp \
    routingtable.cpp \
    \
    internal/internalsubscribecovrequesthandler.cpp \
    internal/internalwprequesthandler.cpp \
    internal/internalunconfirmedrequesthandler.cpp \
    internal/internalwhoisrequesthandler.cpp \
    internal/internalwhohasrequesthandler.cpp \
    internal/internalsubscribecovservicehandler.cpp \
    internal/internalrprequesthandler.cpp \
    internal/bacnetobject2.cpp \
    internal/deviceobject.cpp \
    internal/bacnetproperty.cpp \
    internal/internalconfirmedrequesthandler.cpp \
    internal/multipleasynchhelper.cpp \
    \
    external/externalservicehandler.cpp \
    \
    applayer/remoteobjectstodevicemapper.cpp

HEADERS += bacnetvirtuallinklayer.h \
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
    bacnettagparser.h \
    property.h \
#    bacnetwritepropertyservice.h \
    bacnetwritepropertyservicehandler.h \
    externalobjectshandler.h \
    bacnettsm2.h \ # asynchronousconfirmedhandler.h \
    internalobjectshandler.h \
    asynchsetter.h \
    bacnettsm.h \
    bacnetreadpropertyack.h \
    bacnetreadpropertyservicehandler.h \
    cdm.h \
    bacnetservice.h \ # bacneterrorack.h \
# bacnetreadpropertyservice.h \
    bacnetexternalobjects.h \
    asynchowner.h \
    analoginputobject.h \
    bacnetobjectinternalsupport.h \
    bacnetdeviceobject.h \
    bacnetobject.h \
    servicefactory.h \
    asynchronousbacnettsmaction.h \
    error.h \
    bacnetprimitivedata.h \
    bacnetdefaultobject.h \
    bacnetdataabstract.h \
    bacnetconstructeddata.h \
    propertyvalue.h \
    bacnetsubscribecov.h \
    bacnetunconfirmedcov.h \
    sequenceof.h \
    bacnetconfirmedcov.h \
    bacnetdata.h \
    bacnetserviceack.h \
    propertyowner.h \
    propertyfactory.h \ # check.h \
    cos.h \
    readpropertyservicedata.h \
    bacnetinternaladdresshelper.h \
    whoisservicedata.h \
    bacnetservicedata.h \
    iamservicedata.h \
    whohasservicedata.h \
    ihaveservicedata.h \
    subscribecovservicedata.h \
    writepropertyservicedata.h \
    covsupport.h \
    covnotificationrequestdata.h \
    covconfnotificationservicehandler.h \
    datavisitor.h \
    covincrementhandlers.h \
    bacnetcovsubscription.h \
    covsubscriptionstimehandler.h \
    discoverywrapper.h \
    invokeidgenerator.h \
    routingtable.h \
    \
    internal/internalconfirmedrequesthandler.h \
    internal/internalunconfirmedrequesthandler.h \
    internal/internalwhoisrequesthandler.h \
    internal/internalwhohasrequesthandler.h \
    internal/internalwprequesthandler.h \
    internal/internalsubscribecovrequesthandler.h \
    internal/internalsubscribecovservicehandler.h \
    internal/internalrprequesthandler.h \
    internal/bacnetobject2.h \
    internal/deviceobject.h \
    internal/bacnetproperty.h \
    internal/multipleasynchhelper.h \
    internal/internalpropertycontainersupport.h \
    \
    external/externalservicehandler.h \
    external/externalconfirmedservicehandler.h \
    \
    applayer/remoteobjectstodevicemapper.h

OTHER_FILES +=










































