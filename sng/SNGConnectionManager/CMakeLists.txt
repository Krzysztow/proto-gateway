#set cmaeke minimum version
cmake_minimum_required(VERSION 2.8)

SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 0)

# set project's name
PROJECT( SNGConnectionManager )

# with SET() command you can change variables or define new ones
# here we define SNGCONNECTIONMANAGER_SRCS variable that contains a list of all .cpp files
# note that we don't need  at the end of line
SET( SNGCONNECTIONMANAGER_SRCS
		connectionmanager.cpp
    connectionframe.cpp 
    objectconnectionsupport.cpp 
    groupaddress.cpp
)

# another list, this time it includes all header files that should be treated with moc
SET( SNGCONNECTIONMANAGER_HEADERS
		../sng_include/global.h 
#    ../sng_include/connectionmanager.h 
    ../sng_include/connectionframe.h 
    ../sng_include/objectconnectionsupport.h 
    ../sng_include/groupaddress.h
	)

SET( SNGCONNECTIONMANAGER_MOC_HDRS
	../sng_include/connectionmanager.h
)

# enable warnings
ADD_DEFINITIONS( -Wall )

ADD_DEFINITIONS(-DSNGCONNECTIONMANAGER_LIBRARY)
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)


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

# this command will generate rules that will run rcc on all files from SNGCONNECTIONMANAGER_RCS
# in result SNGCONNECTIONMANAGER_RC_SRCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( SNGCONNECTIONMANAGER_RC_SRCS ${SNGCONNECTIONMANAGER_RCS} )

# this will run uic on .ui files:
QT4_WRAP_UI( SNGCONNECTIONMANAGER_UI_HDRS ${SNGCONNECTIONMANAGER_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( SNGCONNECTIONMANAGER_MOC_SRCS ${SNGCONNECTIONMANAGER_MOC_HDRS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} 
											./
											../sng_include
											)

# here we instruct CMake to build "BACProject" executable from all of the source files
#ADD_EXECUTABLE( BACProject ${SNGCONNECTIONMANAGER_SRCS} ${SNGCONNECTIONMANAGER_MOC_SRCS} ${SNGCONNECTIONMANAGER_RC_SRCS} ${SNGCONNECTIONMANAGER_UI_HDRS} )
ADD_LIBRARY(SNGConnectionManager SHARED ${SNGCONNECTIONMANAGER_SRCS} ${SNGCONNECTIONMANAGER_MOC_SRCS})

# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
TARGET_LINK_LIBRARIES( SNGConnectionManager ${QT_LIBRARIES} )
