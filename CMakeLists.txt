PROJECT(sviewgl)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)
INCLUDE(CheckLibraryExists)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#SET( CMAKE_CXX_COMPILER "g++" )
#SET( CMAKE_C_COMPILER "gcc" )
SET( CMAKE_CXX_FLAGS "-std=c++11" )

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(PNG)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(TIFF)
FIND_PACKAGE(GIF)
FIND_PACKAGE(LIBCONFIG)
FIND_PACKAGE(ZLIB)
FIND_PACKAGE(Imlib2)

IF(LIBCONFIG_FOUND)
    ADD_DEFINITIONS(-DLIBCONFIG_SUPPORT=1)
ENDIF()

IF(IMLIB2_FOUND)
    ADD_DEFINITIONS(-DIMLIB2_SUPPORT=1)
ENDIF()

CHECK_LIBRARY_EXISTS(dl dlclose "" HAVE_DL_LIBRARY)
IF(HAVE_DL_LIBRARY)
    SET(DL_LIBRARY "dl")
ENDIF(HAVE_DL_LIBRARY)

CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_RT_LIBRARY)
IF(HAVE_RT_LIBRARY)
    SET(RT_LIBRARY "rt")
ENDIF(HAVE_RT_LIBRARY)

INCLUDE_DIRECTORIES(
    ${GLFW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    "/usr/X11/include/"
    ${OPENGL_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    )

IF(IMLIB2_FOUND)
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES}
    ${IMLIB2_INCLUDE_DIR}
    )
ENDIF()

IF(LIBCONFIG_FOUND)
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES}
    ${LIBCONFIG_INCLUDE_DIR}
    )
ENDIF()

#LINK_DIRECTORIES(
    #${FREETYPE_LIBRARY_DIRS}
    #)

ADD_EXECUTABLE( sviewgl
    src/checkerboard.cpp
    src/config.cpp
    src/fileslist.cpp
    src/formats/file.cpp
    src/formats/file_zlib.cpp
    src/formats/format.cpp
    src/formats/formatcommon.cpp
    src/formats/formatdds.cpp
    src/formats/formatgif.cpp
    src/formats/formatico.cpp
    src/formats/formatjpeg.cpp
    src/formats/formatpng.cpp
    src/formats/formatppm.cpp
    src/formats/formatpsd.cpp
    src/formats/formatpvr.cpp
    src/formats/formatraw.cpp
    src/formats/formatraw_old.cpp
    src/formats/formatscr.cpp
    src/formats/formattiff.cpp
    src/formats/formatxwd.cpp
    src/formats/helpers.cpp
    src/formats/rle.cpp
    src/ftstring.cpp
    src/ftsymbol.cpp
    src/imageborder.cpp
    src/imageloader.cpp
    src/infobar.cpp
    src/main.cpp
    src/notavailable.cpp
    src/pixelinfo.cpp
    src/progress.cpp
    src/quad.cpp
    src/quadimage.cpp
    src/quadseries.cpp
    src/renderer.cpp
    src/selection.cpp
    src/viewer.cpp
    )

TARGET_LINK_LIBRARIES( sviewgl
    ${RT_LIBRARY}
    ${DL_LIBRARY}
    ${GLFW_STATIC_LIBRARIES}
    ${FREETYPE_LIBRARY}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${TIFF_LIBRARY}
    ${GIF_LIBRARY}
    ${ZLIB_LIBRARIES}
    )

IF(LIBCONFIG_FOUND)
TARGET_LINK_LIBRARIES( sviewgl
    ${LIBCONFIG_LIBRARY}
    )
ENDIF()

IF(IMLIB2_FOUND)
TARGET_LINK_LIBRARIES( sviewgl
    ${IMLIB2_LIBRARY}
    )
ENDIF()

