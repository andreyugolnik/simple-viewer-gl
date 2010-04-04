CC=g++
CFLAGS=-O2 -c -Wall -I/usr/include
LDFLAGS=-s -lGL -lglut -lImlib2

SOURCES= \
	src/checkerboard.cpp \
	src/fileslist.cpp \
	src/imageloader.cpp \
	src/infobar.cpp \
	src/main.cpp \
	src/window.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sviewgl

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm src/*.o
