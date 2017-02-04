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

find_package(OpenGL)
find_package(GLFW)
find_package(Freetype)
find_package(PNG)
find_package(JPEG)
find_package(TIFF)
find_package(GIF)
find_package(ZLIB)
find_package(Imlib2)
find_package(WebP)
find_package(LCMS2)
find_package(X11)
find_package(Threads)

if(IMLIB2_FOUND)
    add_definitions(-DIMLIB2_SUPPORT=1)
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
    )

if(IMLIB2_FOUND)
include_directories( ${INCLUDE_DIRECTORIES}
    ${IMLIB2_INCLUDE_DIR}
    )
endif()

file( GLOB_RECURSE SOURCES "src/*.cpp" )

add_executable( sviewgl ${SOURCES} )

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
    )

if(IMLIB2_FOUND)
target_link_libraries( sviewgl
    ${IMLIB2_LIBRARY}
    )
endif()

