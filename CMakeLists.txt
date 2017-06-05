project(sviewgl)
cmake_minimum_required(VERSION 2.6)

include(FindPkgConfig)
include(CheckLibraryExists)
include(cmake/clang-cxx-dev-tools.cmake)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    message(STATUS "=== RELEASE MODE ===")
    add_definitions( "-DNDEBUG" )
    add_definitions( "-Wall -Wextra -pedantic -pedantic-errors -O2" )
else()
    message(STATUS "=== DEBUG MODE ===")
    add_definitions( "-DDEBUG" )
    add_definitions( "-Wall -Wextra -pedantic -pedantic-errors -O0 -g" )
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(PkgConfig)
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
find_package(X11)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

pkg_check_modules(EXIF REQUIRED libexif)

find_package(Imlib2)
if(IMLIB2_FOUND)
    message(STATUS "ImLib2 support enabled")
    add_definitions(-DIMLIB2_SUPPORT=1)
endif()

pkg_check_modules(OPENEXR OpenEXR)
if(OPENEXR_FOUND)
    pkg_check_modules(ILMBASE REQUIRED IlmBase)
    if(ILMBASE_FOUND)
        message(STATUS "OpenEXR support enabled")
        add_definitions(-DOPENEXR_SUPPORT=1)
    endif()
endif()

check_library_exists(rt clock_gettime "" HAVE_RT_LIBRARY)
if(HAVE_RT_LIBRARY)
    set(RT_LIBRARY "rt")
endif(HAVE_RT_LIBRARY)

include_directories(
    ${EXIF_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${LCMS2_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${WEBP_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    )

if(IMLIB2_FOUND)
    include_directories( ${INCLUDE_DIRECTORIES}
        ${IMLIB2_INCLUDE_DIR}
        )
endif()

if(OPENEXR_FOUND AND ILMBASE_FOUND)
    include_directories( ${INCLUDE_DIRECTORIES}
        ${ILMBASE_INCLUDE_DIRS}
        ${OPENEXR_INCLUDE_DIRS}
        )
endif()

link_directories(
    ${EXIF_LIBRARY_DIRS}
    )

file( GLOB_RECURSE SVIEW_SOURCES "src/*.cpp" )

set(APP_TITLE           "Simple Viewer GL" CACHE STRING "Application name")
set(APP_VERSION_MAJOR   "1" CACHE STRING "Application major version")
set(APP_VERSION_MINOR   "0" CACHE STRING "Application minor version")
set(APP_VERSION_RELEASE "0" CACHE STRING "Application release version")
set(VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}${APP_VERSION_RELEASE}")
message(STATUS "Version ${VERSION}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h"
    )

if(APPLE)
    set(SVIEW_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/macos/Icon.icns)
    set_source_files_properties(${SVIEW_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(sviewgl MACOSX_BUNDLE ${SVIEW_ICON} ${SVIEW_SOURCES})

    set_target_properties(sviewgl PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/res/macos/Info.plist.in
        MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
        )
else()
    add_executable(sviewgl ${SVIEW_SOURCES})
endif()

target_link_libraries( sviewgl
    ${RT_LIBRARY}
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

if(X11_Xinerama_FOUND)
    target_link_libraries( sviewgl
        ${X11_Xinerama_LIB}
        )
endif()

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

