PROJECT(sviewgl)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)
INCLUDE(CheckLibraryExists)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

IF( CMAKE_BUILD_TYPE STREQUAL "Release" )
    MESSAGE(STATUS "=== RELEASE MODE ===")
    ADD_DEFINITIONS( "-DNDEBUG" )
    ADD_DEFINITIONS( "-Wall -O2" )
ELSE()
    MESSAGE(STATUS "=== DEBUG MODE ===")
    ADD_DEFINITIONS( "-DDEBUG" )
    ADD_DEFINITIONS( "-Wall -Wextra -pedantic -O0 -g" )
ENDIF()

SET( CMAKE_CXX_FLAGS "-std=c++14" )

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(GLFW REQUIRED glfw3)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(PNG)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(TIFF)
FIND_PACKAGE(GIF)
FIND_PACKAGE(ZLIB)
FIND_PACKAGE(Imlib2)
FIND_PACKAGE(WebP)
FIND_PACKAGE(X11)


FIND_PACKAGE(Threads)

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
    ${X11_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${WEBP_INCLUDE_DIRS}
    )

IF(IMLIB2_FOUND)
INCLUDE_DIRECTORIES( ${INCLUDE_DIRECTORIES}
    ${IMLIB2_INCLUDE_DIR}
    )
ENDIF()

#LINK_DIRECTORIES(
    #${FREETYPE_LIBRARY_DIRS}
    #)

ADD_EXECUTABLE( sviewgl
    src/checkerboard.cpp
    src/common/Ini.cpp
    src/common/ZlibDecoder.cpp
    src/common/config.cpp
    src/common/file.cpp
    src/common/file_zlib.cpp
    src/common/helpers.cpp
    src/fileslist.cpp
    src/formats/AGEheader.cpp
    src/formats/format.cpp
    src/formats/formatage.cpp
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
    src/formats/formatscr.cpp
    src/formats/formattarga.cpp
    src/formats/formattiff.cpp
    src/formats/formatwebp.cpp
    src/formats/formatxpm.cpp
    src/formats/formatxwd.cpp
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
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLFW_STATIC_LIBRARIES}
    ${FREETYPE_LIBRARY}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${TIFF_LIBRARY}
    ${GIF_LIBRARY}
    ${ZLIB_LIBRARIES}
    ${WEBP_LIBRARIES}
    )

IF(IMLIB2_FOUND)
TARGET_LINK_LIBRARIES( sviewgl
    ${IMLIB2_LIBRARY}
    )
ENDIF()

