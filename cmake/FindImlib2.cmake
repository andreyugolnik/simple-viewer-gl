# Find Imlib2 library
#
# Find the native Imlib2 includes and library
# This module defines:
#  IMLIB2_INCLUDE_DIR, where to find Imlib2.h, etc.
#  IMLIB2_LIBRARIES, libraries to link against to use Imlib2.
#  IMLIB2_FOUND, If false, do not try to use Imlib2.
# Also defined, but not for general use are:
#  IMLIB2_LIBRARY, where to find the Imlib library.

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

FIND_PATH(IMLIB2_INCLUDE_DIR Imlib2.h
	/usr/include
	/usr/local/include
	~/Library/Frameworks
	/Library/Frameworks
)

FIND_LIBRARY(IMLIB2_LIBRARY
	NAMES libImlib2 Imlib2
	/usr/lib
	/usr/local/lib
	~/Library/Frameworks
	/Library/Frameworks
	)

# handle the QUIETLY and REQUIRED arguments and set IMLIB2_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Imlib2  DEFAULT_MSG  IMLIB2_LIBRARY  IMLIB2_INCLUDE_DIR)

IF(IMLIB2_FOUND)
  SET(IMLIB2_LIBRARIES ${IMLIB2_LIBRARY})
ENDIF(IMLIB2_FOUND)

MARK_AS_ADVANCED(IMLIB2_INCLUDE_DIR IMLIB2_LIBRARY)
