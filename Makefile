CC=g++
CFLAGS=-O2 -c -Wall -I/usr/include -I/usr/include/freetype2
LDFLAGS=-s -lGL -lglut -lfreetype -ljpeg `libpng-config --libs` -lgif -ltiff -lImlib2

SOURCES= \
	src/formats/format.cpp \
	src/formats/formatcommon.cpp \
	src/formats/formatjpeg.cpp \
	src/formats/formatpsd.cpp \
	src/formats/formatpng.cpp \
	src/formats/formatgif.cpp \
	src/formats/formatico.cpp \
	src/formats/formattiff.cpp \
	src/ftstring.cpp \
	src/ftsymbol.cpp \
	src/imageborder.cpp \
	src/checkerboard.cpp \
	src/notavailable.cpp \
	src/progress.cpp \
	src/quad.cpp \
	src/quadimage.cpp \
	src/fileslist.cpp \
	src/imageloader.cpp \
	src/infobar.cpp \
	src/pixelinfo.cpp \
	src/main.cpp \
	src/selection.cpp \
	src/window.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sviewgl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
