project(sviewgl)
cmake_minimum_required(VERSION 2.6)

include(FindPkgConfig)
include(CheckLibraryExists)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    message(STATUS "=== RELEASE MODE ===")
    add_definitions( "-DNDEBUG" )
    add_definitions( "-Wall -O2" )
else()
    message(STATUS "=== DEBUG MODE ===")
    add_definitions( "-DDEBUG" )
    add_definitions( "-Wall -Wextra -pedantic -O0 -g" )
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(TIFF REQUIRED)
find_package(GIF REQUIRED)
find_package(ZLIB REQUIRED)
find_package(WebP REQUIRED)
find_package(LCMS2 REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(PkgConfig)
pkg_check_modules(EXIF libexif)

find_package(X11)

find_package(Imlib2)
if(IMLIB2_FOUND)
    add_definitions(-DIMLIB2_SUPPORT=1)
endif()

find_package(OpenEXR)
if(OPENEXR_FOUND)
    find_package(ILMBase)
    if(ILMBASE_FOUND)
        add_definitions(-DOPENEXR_SUPPORT=1)
    endif()
endif()

check_library_exists(dl dlclose "" HAVE_DL_LIBRARY)
if(HAVE_DL_LIBRARY)
    set(DL_LIBRARY "dl")
endif(HAVE_DL_LIBRARY)

check_library_exists(rt clock_gettime "" HAVE_RT_LIBRARY)
if(HAVE_RT_LIBRARY)
    set(RT_LIBRARY "rt")
endif(HAVE_RT_LIBRARY)

include_directories(
    ${X11_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${WEBP_INCLUDE_DIRS}
    ${LCMS2_INCLUDE_DIR}
    ${EXIF_INCLUDE_DIR}
    )

if(IMLIB2_FOUND)
    include_directories( ${INCLUDE_DIRECTORIES}
        ${IMLIB2_INCLUDE_DIR}
        )
endif()

if(OPENEXR_FOUND AND ILMBASE_FOUND)
    include_directories( ${INCLUDE_DIRECTORIES}
        ${OPENEXR_INCLUDE_DIR}
        ${ILMBASE_INCLUDE_DIR}
        )
endif()

file( GLOB_RECURSE SVIEW_SOURCES "src/*.cpp" )

set(APP_TITLE           "Simple Viewer GL")
set(APP_VERSION_MAJOR   "2")
set(APP_VERSION_MINOR   "8")
set(APP_VERSION_RELEASE "9")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h"
    )

if(APPLE)
    set(SVIEW_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/macos/Icon.icns)
    set_source_files_properties(${SVIEW_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(sviewgl MACOSX_BUNDLE ${SVIEW_ICON} ${SVIEW_SOURCES})

    set(VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}${APP_VERSION_RELEASE}")
    set_target_properties(sviewgl PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/res/macos/Info.plist.in
        MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
        )
else()
    add_executable(sviewgl ${SVIEW_SOURCES})
endif()

#link_directories(
#${FREETYPE_LIBRARY_DIRS}
#)

target_link_libraries( sviewgl
    ${RT_LIBRARY}
    ${DL_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${X11_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${ZLIB_LIBRARIES}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${TIFF_LIBRARY}
    ${GIF_LIBRARY}
    ${WEBP_LIBRARIES}
    ${FREETYPE_LIBRARY}
    ${LCMS2_LIBRARIES}
    ${EXIF_LIBRARIES}
    )

if(IMLIB2_FOUND)
    target_link_libraries( sviewgl
        ${IMLIB2_LIBRARY}
        )
endif()

if(OPENEXR_FOUND AND ILMBASE_FOUND)
    target_link_libraries( sviewgl
        ${ILMBASE_LIBRARIES}
        ${OPENEXR_LIBRARIES}
        )
endif()

