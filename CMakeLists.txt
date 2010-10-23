PROJECT(sviewgl)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# additional compiler flags
ADD_DEFINITIONS(-Wall -O2)

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Freetype)
FIND_PACKAGE(PNG)
FIND_PACKAGE(JPEG)
FIND_PACKAGE(TIFF)
FIND_PACKAGE(GIF)
FIND_PACKAGE(IMLIB2)
FIND_PACKAGE(LIBCONFIG)

INCLUDE_DIRECTORIES(
	${FREETYPE_INCLUDE_DIRS}
	${GLUT_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	)

#LINK_DIRECTORIES(
#	${FREETYPE_LIBRARY_DIRS}
#	)

ADD_EXECUTABLE(sviewgl
	src/checkerboard.cpp
	src/ftsymbol.cpp
	src/main.cpp
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
	src/formats/formatgif.cpp
	src/formats/formatpng.cpp
	)

TARGET_LINK_LIBRARIES(sviewgl
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

