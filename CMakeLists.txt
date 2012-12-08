PROJECT(sviewgl)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)
INCLUDE(CheckLibraryExists)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#SET( CMAKE_CXX_COMPILER "clang++" )
#SET( CMAKE_C_COMPILER "clang" )

# additional compiler flags
ADD_DEFINITIONS(-Wall -O0 -g)
#ADD_DEFINITIONS(-Wall -Wextra -O2)

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(PNG)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(TIFF)
FIND_PACKAGE(GIF)
FIND_PACKAGE(IMLIB2)
FIND_PACKAGE(LIBCONFIG)

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
    )

#LINK_DIRECTORIES(
    #${FREETYPE_LIBRARY_DIRS}
    #)

ADD_EXECUTABLE(../sviewgl
    src/checkerboard.cpp
    src/ftsymbol.cpp
    src/main.cpp
    src/renderer.cpp
    src/quad.cpp
    src/window.cpp
    src/config.cpp
    src/imageborder.cpp
    src/notavailable.cpp
    src/quadimage.cpp
    src/fileslist.cpp
    src/imageloader.cpp
    src/pixelinfo.cpp
    src/quadseries.cpp
    src/ftstring.cpp
    src/infobar.cpp
    src/progress.cpp
    src/selection.cpp
    src/formats/formatcommon.cpp
    src/formats/formatico.cpp
    src/formats/formatpsd.cpp
    src/formats/format.cpp
    src/formats/formatjpeg.cpp
    src/formats/formattiff.cpp
    src/formats/formatxwd.cpp
    #src/formats/formatsvg.cpp
    src/formats/formatdds.cpp
    src/formats/formatgif.cpp
    src/formats/formatpng.cpp
    src/formats/formatraw.cpp
    src/math/vector.cpp
    )

TARGET_LINK_LIBRARIES(../sviewgl
    ${RT_LIBRARY}
    ${FREETYPE_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLUT_LIBRARY}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${TIFF_LIBRARY}
    ${GIF_LIBRARY}
    ${IMLIB2_LIBRARY}
    ${LIBCONFIG_LIBRARY}
    )

