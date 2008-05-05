# - Find Taglib library
# Find the Taglib includes and library
# This module defines
#  TAGLIB_INCLUDE_DIR, where to find taglib.h, etc.
#  TAGLIB_LIBRARY_DIR, where to find tag.lib
#  TAGLIB_LIBRARIES, libraries to link against to use Taglib.
#  TAGLIB_FOUND, If false, do not try to use Taglib.
# also defined, but not for general use are
#  TAGLIB_LIBRARY, where to find the Taglib library.

FIND_PATH(TAGLIB_INCLUDE_DIR taglib.h
  /usr/local/include/taglib
  /usr/include/taglib
  $ENV{TAGLIB_DIR}/include/taglib
)

FIND_PATH(TAGLIB_LIBRARY_DIR tag.lib
  /usr/local/include
  /usr/include
  $ENV{TAGLIB_DIR}/lib
)

FIND_LIBRARY(TAGLIB_LIBRARY
  NAMES tag
  PATHS /usr/lib /usr/local/lib $ENV{TAGLIB_DIR}/lib
  )

IF(TAGLIB_INCLUDE_DIR)
  IF(TAGLIB_LIBRARY)
    SET( TAGLIB_FOUND "YES" )
    SET( TAGLIB_LIBRARIES ${TAGLIB_LIBRARY} )
    MESSAGE(STATUS "Found Taglib")
  ELSE(TAGLIB_LIBRARY)
    MESSAGE(SEND_ERROR "Taglib library file not found!")
  ENDIF(TAGLIB_LIBRARY)
ELSE(TAGLIB_INCLUDE_DIR)
  MESSAGE(SEND_ERROR "Taglib includes not found! Please install taglib and set TAGLIB_DIR to the path of installation")
ENDIF(TAGLIB_INCLUDE_DIR)
