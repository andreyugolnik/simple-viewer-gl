# - Try to find libOPENJPEG
# Once done, this will define
#
#  OPENJPEG_FOUND - system has asdcplib
#  OPENJPEG_INCLUDE_DIRS - the asdcplib include directories
#  OPENJPEG_LIBRARIES - link these to use asdcplib

# Include dir
FIND_PATH(OPENJPEG_INCLUDE_DIR openjpeg.h
    /usr/local/include/openjpeg
    /usr/local/include/openjpeg-2.3
    /usr/local/include
    /usr/include/openjpeg
    /usr/include/openjpeg-2.3
    /usr/include
    )

SET(OPENJPEG_NAMES ${OPENJPEG_NAMES} openjp2 openjpeg)
FIND_LIBRARY(OPENJPEG_LIBRARY
    NAMES ${OPENJPEG_NAMES}
    PATHS /usr/lib /usr/local/lib
    )

IF (OPENJPEG_LIBRARY AND OPENJPEG_INCLUDE_DIR)
    SET(OPENJPEG_LIBRARIES ${OPENJPEG_LIBRARY})
    SET(OPENJPEG_FOUND "YES")
ENDIF (OPENJPEG_LIBRARY AND OPENJPEG_INCLUDE_DIR)


IF (OPENJPEG_FOUND)
    IF (NOT OPENJPEG_FIND_QUIETLY)
        MESSAGE(STATUS "Found OpenJPEG: ${OPENJPEG_LIBRARIES}")
    ENDIF (NOT OPENJPEG_FIND_QUIETLY)
ELSE (OPENJPEG_FOUND)
    IF (OPENJPEG_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find OpenJPEG library")
    ENDIF (OPENJPEG_FIND_REQUIRED)
ENDIF (OPENJPEG_FOUND)

# Deprecated declarations.
SET (NATIVE_OPENJPEG_INCLUDE_PATH ${OPENJPEG_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_OPENJPEG_LIB_PATH ${OPENJPEG_LIBRARY} PATH)
