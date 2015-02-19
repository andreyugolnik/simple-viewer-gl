PROJECT(sviewgl)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)
INCLUDE(CheckLibraryExists)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#SET( CMAKE_CXX_COMPILER "g++" )
#SET( CMAKE_C_COMPILER "gcc" )

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(PNG)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(TIFF)
FIND_PACKAGE(GIF)
FIND_PACKAGE(LIBCONFIG)
FIND_PACKAGE(ZLIB)
FIND_PACKAGE(Imlib2)
if(IMLIB2_FOUND)
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
    ${FREETYPE_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIR}
    "/usr/X11/include/"
    ${OPENGL_INCLUDE_DIR}
    ${LIBCONFIG_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    )

if(IMLIB2_FOUND)
INCLUDE_DIRECTORIES(
    ${INCLUDE_DIRECTORIES}
    ${IMLIB2_INCLUDE_DIR}
    )
ENDIF()

#LINK_DIRECTORIES(
    #${FREETYPE_LIBRARY_DIRS}
    #)

ADD_EXECUTABLE(sviewgl
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
    src/window.cpp
    )

TARGET_LINK_LIBRARIES(sviewgl
    ${RT_LIBRARY}
    ${DL_LIBRARY}
    ${FREETYPE_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLUT_LIBRARY}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${TIFF_LIBRARY}
    ${GIF_LIBRARY}
    ${LIBCONFIG_LIBRARY}
    ${ZLIB_LIBRARIES}
    )

if(IMLIB2_FOUND)
TARGET_LINK_LIBRARIES(sviewgl
    ${IMLIB2_LIBRARY}
    )
ENDIF()

