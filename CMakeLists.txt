#set cmaeke minimum version
cmake_minimum_required(VERSION 2.8)

# set project's name
PROJECT( cmake_test )

# with SET() command you can change variables or define new ones
# here we define BACNET_SRCS variable that contains a list of all .cpp files
# note that we don't need  at the end of line
SET( BACNET_SRCS
    main.cpp 
    bacnetvirtuallinklayer.cpp 
    bacnetnetworklayer.cpp 
    bacnetbbmdhandler.cpp 
    bacnetbipaddress.cpp 
    bacnetudptransportlayer.cpp 
    bacnetbiptransportlayer.cpp 
    bacnetaddress.cpp 
    bacnetrouter.cpp 
    bacnetapplicationlayer.cpp 
    bacnetnpci.cpp 
    helpercoder.cpp 
    bacnetbuffermanager.cpp 
    buffer.cpp 
    bacnetpci.cpp 
    bacnetcoder.cpp 
    services/bacnetreadproperty.cpp 
    bacnettagparser.cpp 
    property.cpp 
    asynchowner.cpp 
    bacnettsm2.cpp 
    cdm.cpp 
    bacnettsm.cpp 
    bacnetreadpropertyack.cpp 
    bacnetcommon.cpp 
    bacnetobjectinternalsupport.cpp 
    analoginputobject.cpp 
    bacnetdeviceobject.cpp 
    bacnetobject.cpp 
    bacnetprimitivedata.cpp
    servicefactory.cpp 
    asynchronousbacnettsmaction.cpp 
    bacnetservice.cpp 
    error.cpp 
    propertyvalue.cpp 
    bacnetdataabstract.cpp 
    bacnetconstructeddata.cpp 
    bacnetdefaultobject.cpp 
    bacnetsubscribecov.cpp 
    bacnetunconfirmedcov.cpp 
    bacnetconfirmedcov.cpp 
    sequenceof.cpp 
    bacnetdata.cpp 
    bacnetserviceack.cpp 
    propertyfactory.cpp 
    bacnetinternaladdresshelper.cpp 
    covsupport.cpp 
    covconfnotificationservicehandler.cpp 
    datavisitor.cpp 
    covincrementhandlers.cpp 
    bacnetcovsubscription.cpp 
    covsubscriptionstimehandler.cpp 
    discoverywrapper.cpp 
    invokeidgenerator.cpp 
    routingtable.cpp 
    #
    internal/internalsubscribecovrequesthandler.cpp 
    internal/internalwprequesthandler.cpp 
    internal/internalunconfirmedrequesthandler.cpp 
    internal/internalwhoisrequesthandler.cpp 
    internal/internalwhohasrequesthandler.cpp 
    internal/internalobjectshandler.cpp 
    internal/internalsubscribecovservicehandler.cpp 
    internal/internalrprequesthandler.cpp 
    internal/bacnetobject2.cpp 
    internal/deviceobject.cpp 
    internal/bacnetproperty.cpp 
    internal/internalconfirmedrequesthandler.cpp 
    internal/multipleasynchhelper.cpp 
    internal/internalihaveservicehandler.cpp 
    #
    external/externalservicehandler.cpp 
    external/bacnetwritepropertyservicehandler.cpp 
    external/bacnetreadpropertyservicehandler.cpp 
    external/externalobjectshandler.cpp 
    #
    applayer/writepropertyservicedata.cpp 
    applayer/whoisservicedata.cpp 
    applayer/whohasservicedata.cpp 
    applayer/subscribecovservicedata.cpp 
    applayer/readpropertyservicedata.cpp 
    applayer/ihaveservicedata.cpp 
    applayer/remoteobjectstodevicemapper.cpp 
    applayer/iamservicedata.cpp 
    applayer/covnotificationrequestdata.cpp 
    internal/internaliamservicehandler.cpp 
    externalconfirmedservicewrapper.cpp 
    external/externalpropertymapping.cpp 
    external/externalobjectreadstrategy.cpp 
    external/subscribecovservicehandler.cpp 
    tests/covanswerer.cpp 
    internal/internalconfirmedcovnotifhandler.cpp 
    applayer/bacnetarrayvisitor.cpp 
    internal/internaluncfrdmcovnotifhandler.cpp 
    tests/rpanswerer.cpp 
    external/externalobjectwritestrategy.cpp 
    external/externaltimedepjob.cpp 
    tests/wprequester.cpp 
    tests/wpacknowledger.cpp 
    factories/bacnetconfigurator.cpp 
    factories/networklayerconfigurator.cpp 
    factories/transportlayerconfigurator.cpp 
    factories/configuratorhelper.cpp
)

# another list, this time it includes all header files that should be treated with moc
SET( BACNET_HEADERS
     bacnetvirtuallinklayer.h 
    bacnetnetworklayer.h 
    bacnetbbmdhandler.h 
    bacnetbipaddress.h  
    bacnetcommon.h 
    bacnettransportlayer.h 
    bacnetbiptransportlayer.h 
    bacnetaddress.h
    bitfields.h 
    bacnetnpci.h 
    helpercoder.h 
    bacnetbuffermanager.h 
    buffer.h 
    bacnetpci.h 
    bacnetcoder.h 
    services/bacnetreadproperty.h 
    bacnettagparser.h 
    property.h  
    asynchsetter.h 
    bacnettsm.h 
    bacnetreadpropertyack.h
    bacnetservice.h 
    analoginputobject.h 
    bacnetobjectinternalsupport.h 
    bacnetdeviceobject.h 
    bacnetobject.h 
    servicefactory.h 
    asynchronousbacnettsmaction.h 
    error.h 
    bacnetprimitivedata.h 
    bacnetdefaultobject.h 
    bacnetdataabstract.h 
    bacnetconstructeddata.h 
    propertyvalue.h 
    bacnetsubscribecov.h 
    bacnetunconfirmedcov.h 
    sequenceof.h 
    bacnetconfirmedcov.h 
    bacnetdata.h 
    bacnetserviceack.h 
    propertyowner.h 
    propertyfactory.h 
    cos.h 
    bacnetinternaladdresshelper.h 
    covsupport.h 
    covconfnotificationservicehandler.h 
    datavisitor.h 
    covincrementhandlers.h 
    bacnetcovsubscription.h 
    discoverywrapper.h 
    invokeidgenerator.h 
    routingtable.h 
    #
    internal/internalconfirmedrequesthandler.h 
    internal/internalunconfirmedrequesthandler.h 
    internal/internalwhoisrequesthandler.h 
    internal/internalwhohasrequesthandler.h 
    internal/internalwprequesthandler.h 
    internal/internalsubscribecovrequesthandler.h 
    internal/internalsubscribecovservicehandler.h 
    internal/internalobjectshandler.h 
    internal/internalrprequesthandler.h 
    internal/bacnetobject2.h 
    internal/deviceobject.h 
    internal/bacnetproperty.h 
    internal/multipleasynchhelper.h 
    internal/internalpropertycontainersupport.h 
    internal/internalihaveservicehandler.h
    #
    external/externalservicehandler.h 
    external/externalconfirmedservicehandler.h 
    external/bacnetwritepropertyservicehandler.h
    external/bacnetreadpropertyservicehandler.h 
    #
    applayer/writepropertyservicedata.h 
    applayer/whoisservicedata.h 
    applayer/whohasservicedata.h 
    applayer/subscribecovservicedata.h 
    applayer/readpropertyservicedata.h 
    applayer/ihaveservicedata.h 
    applayer/remoteobjectstodevicemapper.h 
    applayer/iamservicedata.h 
    applayer/covnotificationrequestdata.h 
    applayer/bacnetservicedata.h 
    internal/internaliamservicehandler.h 
    externalconfirmedservicewrapper.h 
    external/externalpropertymapping.h 
    external/externalobjectreadstrategy.h 
    external/subscribecovservicehandler.h 
    internal/internalconfirmedcovnotifhandler.h 
    applayer/bacnetarrayvisitor.h 
    internal/internaluncfrdmcovnotifhandler.h
    external/externalobjectwritestrategy.h 
    external/externaltimedepjob.h 
    factories/bacnetconfigurator.h 
    factories/networklayerconfigurator.h 
    factories/transportlayerconfigurator.h 
    factories/configuratorhelper.h
	)

SET( BACNET_MOC_HDRS
    asynchowner.h     
    asynchowner.h  
    bacnetapplicationlayer.h  
    bacnetrouter.h
    bacnettsm2.h
    bacnetudptransportlayer.h 
    cdm.h  
    covsubscriptionstimehandler.h 
    external/externalobjectshandler.h 
    tests/covanswerer.h  
    tests/rpanswerer.h 
    tests/wprequester.h 
    tests/wpacknowledger.h 
)

# enable warnings
ADD_DEFINITIONS( -Wall )

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
SET( QT_USE_QTGUI FALSE )
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QT3SUPPORT TRUE )

# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
FIND_PACKAGE( Qt4 REQUIRED )

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )

# this command will generate rules that will run rcc on all files from BACNET_RCS
# in result BACNET_RC_SRCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( BACNET_RC_SRCS ${BACNET_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( BACNET_UI_HDRS ${BACNET_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( BACNET_MOC_SRCS ${BACNET_MOC_HDRS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} 
											./
											./internal 
    									./external 
											./applayer 
											./factories 
											./tests
											)

# here we instruct CMake to build "BACProject" executable from all of the source files
ADD_EXECUTABLE( BACProject ${BACNET_SRCS} ${BACNET_MOC_SRCS} ${BACNET_RC_SRCS} ${BACNET_UI_HDRS} )

# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
TARGET_LINK_LIBRARIES( BACProject ${QT_LIBRARIES} )
