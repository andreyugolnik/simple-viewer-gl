project(sviewgl)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)
include(CheckLibraryExists)
include(cmake/clang-cxx-dev-tools.cmake)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(APP_TITLE           "Simple Viewer GL" CACHE STRING "Application name")
set(APP_VERSION_MAJOR   "1" CACHE STRING "Application major version")
set(APP_VERSION_MINOR   "0" CACHE STRING "Application minor version")
set(APP_VERSION_RELEASE "0" CACHE STRING "Application release version")
set(VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_RELEASE}")
message(STATUS "***************************")
message(STATUS "* Simple Viewer GL v${VERSION} *")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "* Release Build")
    add_definitions("-DNDEBUG" )
    add_definitions("-Wall -Wextra -pedantic -pedantic-errors -O2")
else()
    message(STATUS "* Debug Build")
    add_definitions("-DDEBUG" )
    add_definitions("-Wall -Wextra -pedantic -pedantic-errors -O0 -g")
endif()

find_package(PkgConfig)

find_package(OpenGL QUIET REQUIRED)
if(OPENGL_FOUND)
    message(STATUS "* OPENGL found")
    message(STATUS "  ${OPENGL_INCLUDE_DIR}")
    message(STATUS "  ${OPENGL_LIBRARY}")
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

pkg_check_modules(GLFW3 QUIET REQUIRED glfw3)
if(GLFW3_FOUND)
    message(STATUS "* GLFW3 found")
    message(STATUS "  ${GLFW3_INCLUDE_DIRS}")
    message(STATUS "  ${GLFW3_LIBRARIES}")
    include_directories(${GLFW3_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(THREADS QUIET REQUIRED)
if(THREADS_FOUND)
    message(STATUS "* THREADS found")
endif()

find_package(X11 QUIET)
if(X11_FOUND)
    message(STATUS "* X11 found")
    message(STATUS "  ${X11_INCLUDE_DIR}")
    message(STATUS "  ${X11_LIBRARIES}")
    include_directories(${X11_INCLUDE_DIR})
endif()

find_package(LCMS2 QUIET REQUIRED)
if(LCMS2_FOUND)
    message(STATUS "* LCMS2 found")
    message(STATUS "  ${LCMS2_INCLUDE_DIR}")
    message(STATUS "  ${LCMS2_LIBRARIES}")
    # add_definitions(-DLCMS2_SUPPORT=1)
    include_directories(${LCMS2_INCLUDE_DIR})
endif()

find_package(ZLIB QUIET REQUIRED)
if(ZLIB_FOUND)
    message(STATUS "* ZLIB found")
    message(STATUS "  ${ZLIB_INCLUDE_DIR}")
    message(STATUS "  ${ZLIB_LIBRARIES}")
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

find_package(PNG QUIET REQUIRED)
if(PNG_FOUND)
    message(STATUS "* PNG support enabled")
    message(STATUS "  ${PNG_INCLUDE_DIR}")
    message(STATUS "  ${PNG_LIBRARY}")
    # add_definitions(-DPNG_SUPPORT=1)
    include_directories(${PNG_INCLUDE_DIR})
endif()

find_package(JPEG QUIET REQUIRED)
if(JPEG_FOUND)
    message(STATUS "* JPEG support enabled")
    message(STATUS "  ${JPEG_INCLUDE_DIR}")
    # message(STATUS "  ${JPEG_LIBRARIES}")
    add_definitions(-DJPEG_SUPPORT=1)
    include_directories(${JPEG_INCLUDE_DIR})
endif()

pkg_check_modules(EXIF QUIET REQUIRED libexif)
if(EXIF_FOUND)
    message(STATUS "* EXIF support enabled")
    message(STATUS "  ${EXIF_INCLUDE_DIRS}")
    message(STATUS "  ${EXIF_LIBRARIES}")
    # add_definitions(-DEXIF_SUPPORT=1)
    include_directories(${EXIF_INCLUDE_DIRS})
endif()

pkg_check_modules(JPEG2K QUIET libopenjp2)
if(JPEG2K_FOUND)
    message(STATUS "* OpenJPEG2 support enabled")
    message(STATUS "  ${JPEG2K_INCLUDE_DIRS}")
    message(STATUS "  ${JPEG2K_LIBRARIES}")
    add_definitions(-DJPEG2000_SUPPORT=1)
    include_directories(${JPEG2K_INCLUDE_DIRS})
endif()

find_package(GIF QUIET)
if(GIF_FOUND)
    message(STATUS "* GIF support enabled")
    message(STATUS "  ${GIF_INCLUDE_DIR}")
    message(STATUS "  ${GIF_LIBRARIES}")
    add_definitions(-DGIF_SUPPORT=1)
    include_directories(${GIF_INCLUDE_DIR})
endif()

find_package(TIFF QUIET)
if(TIFF_FOUND)
    message(STATUS "* TIFF support enabled")
    message(STATUS "  ${TIFF_INCLUDE_DIRS}")
    message(STATUS "  ${TIFF_LIBRARIES}")
    add_definitions(-DTIFF_SUPPORT=1)
    include_directories(${TIFF_INCLUDE_DIRS})
endif()

find_package(WEBP QUIET)
if(WEBP_FOUND)
    message(STATUS "* WebP support enabled")
    message(STATUS "  ${WEBP_INCLUDE_DIRS}")
    message(STATUS "  ${WEBP_LIBRARIES}")
    add_definitions(-DWEBP_SUPPORT=1)
    include_directories(${WEBP_INCLUDE_DIRS})
endif()

pkg_check_modules(OPENEXR QUIET OpenEXR)
if(OPENEXR_FOUND)
    pkg_check_modules(ILMBASE REQUIRED IlmBase)
    if(ILMBASE_FOUND)
        message(STATUS "* OpenEXR support enabled")
        message(STATUS "  ${ILMBASE_INCLUDE_DIRS} ${OPENEXR_INCLUDE_DIRS}")
        message(STATUS "  ${ILMBASE_LIBRARIES} ${OPENEXR_LIBRARIES}")
        add_definitions(-DOPENEXR_SUPPORT=1)
        include_directories(${ILMBASE_INCLUDE_DIRS} ${OPENEXR_INCLUDE_DIRS})
    endif()
endif()

find_package(IMLIB2 QUIET)
if(IMLIB2_FOUND)
    message(STATUS "* ImLib2 support enabled")
    message(STATUS "  ${IMLIB2_INCLUDE_DIR}")
    message(STATUS "  ${IMLIB2_LIBRARY}")
    add_definitions(-DIMLIB2_SUPPORT=1)
    include_directories(${IMLIB2_INCLUDE_DIR})
endif()

check_library_exists(rt clock_gettime "" RT_FOUND)
if(RT_FOUND)
    set(RT_LIBRARY "rt")
endif()

include_directories("src")

add_subdirectory(third-party/imgui)
include_directories("third-party/imgui")

add_subdirectory(third-party/xcf)
include_directories("third-party/xcf")

file(GLOB_RECURSE SVIEW_SOURCES "src/*.cpp")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h"
    )

if(APPLE)
    execute_process(
        COMMAND makeicns -in res/Icon-1024.png -32 res/Icon-32.png -16 res/Icon-16.png -out res/macos/Icon.icns
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
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

add_dependencies(sviewgl ImGui)
target_link_libraries(sviewgl ImGui)

add_dependencies(sviewgl XcfTools)
target_link_libraries(sviewgl XcfTools)

if(OPENGL_FOUND)
    target_link_libraries(sviewgl ${OPENGL_LIBRARY})
endif()

if(GLFW3_FOUND)
    target_link_libraries(sviewgl ${GLFW3_LIBRARIES})
endif()

if(LCMS2_FOUND)
    target_link_libraries(sviewgl ${LCMS2_LIBRARIES})
endif()

if(ZLIB_FOUND)
    target_link_libraries(sviewgl ${ZLIB_LIBRARIES})
endif()

if(X11_FOUND)
    target_link_libraries(sviewgl ${X11_LIBRARIES})
endif()

if(X11_Xinerama_FOUND)
    target_link_libraries(sviewgl ${X11_Xinerama_LIB})
endif()

if(PNG_FOUND)
    target_link_libraries(sviewgl ${PNG_LIBRARY})
endif()

if(JPEG_FOUND)
    target_link_libraries(sviewgl ${JPEG_LIBRARIES})
endif()

if(EXIF_FOUND)
    target_link_libraries(sviewgl ${EXIF_LIBRARIES})
endif()

if(JPEG2K_FOUND)
    target_link_libraries(sviewgl ${JPEG2K_LIBRARIES})
endif()

if(GIF_FOUND)
    target_link_libraries(sviewgl ${GIF_LIBRARIES})
endif()

if(TIFF_FOUND)
    target_link_libraries(sviewgl ${TIFF_LIBRARIES})
endif()

if(WEBP_FOUND)
    target_link_libraries(sviewgl ${WEBP_LIBRARIES})
endif()

if(OPENEXR_FOUND AND ILMBASE_FOUND)
    target_link_libraries(sviewgl ${ILMBASE_LIBRARIES} ${OPENEXR_LIBRARIES})
endif()

if(IMLIB2_FOUND)
    target_link_libraries(sviewgl ${IMLIB2_LIBRARY})
endif()

if(RT_FOUND)
    target_link_libraries(sviewgl ${RT_LIBRARY})
endif()
