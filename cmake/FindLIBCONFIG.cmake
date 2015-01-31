# Find libConfig library
#
# Find the native LIBCONFIG includes and library
# This module defines:
#  LIBCONFIG_INCLUDE_DIR, where to find libconfig.h, etc.
#  LIBCONFIG_LIBRARIES, libraries to link against to use libConfig.
#  LIBCONFIG_FOUND, If false, do not try to use libConfig.
# Also defined, but not for general use are:
#  LIBCONFIG_LIBRARY, where to find the Imlib library.

#=============================================================================
# Copyright 2010 Andrey A. Ugolnik.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(LIBCONFIG_INCLUDE_DIR libconfig.h
    /usr/include
    /usr/local/include
    ~/Library/Frameworks
    /Library/Frameworks
)

FIND_LIBRARY(LIBCONFIG_LIBRARY
    NAMES libconfig++ libconfig config++ config
    /usr/lib
    /usr/local/lib
    ~/Library/Frameworks
    /Library/Frameworks
    )

# handle the QUIETLY and REQUIRED arguments and set LIBCONFIG_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBCONFIG  DEFAULT_MSG  LIBCONFIG_LIBRARY  LIBCONFIG_INCLUDE_DIR)

IF(LIBCONFIG_FOUND)
  SET(LIBCONFIG_LIBRARIES ${LIBCONFIG_LIBRARY})
ENDIF(LIBCONFIG_FOUND)

MARK_AS_ADVANCED(LIBCONFIG_INCLUDE_DIR LIBCONFIG_LIBRARY)

