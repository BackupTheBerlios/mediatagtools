project (MTT)

cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(Qt4 REQUIRED)

IF (NOT QT4_FOUND)
	message("Qt4 not found! Try setting QTDIR to where QT4 libs are installed...")
ENDIF(NOT QT4_FOUND)

SET(CMAKE_MODULE_PATH ${MTT_SOURCE_DIR})
FIND_PACKAGE(Taglib REQUIRED)

INCLUDE_DIRECTORIES( ${TAGLIB_INCLUDE_DIR} ${QT_INCLUDE_DIR} )
LINK_DIRECTORIES( ${TAGLIB_LIBRARY_DIR} ${QT_LIBRARY_DIR} )

add_subdirectory(src)
