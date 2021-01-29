CC=g++
CFLAGS=-march=native -std=c++11 -g3 -Wall -O2
DEFINES=-DCL_TARGET_OPENCL_VERSION=120
LDFLAGS=-g3
LIBS=-lOpenCL
RM=/bin/rm

SOURCES=lodepng.cc mandelbrot.cc cl_utils.cc

EXECUTABLE=mandelbrot_opencl

OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cc.o:
	$(CC) -c $(CFLAGS) $(DEFINES) $< -o $@

clean:
	$(RM) -f $(OBJECTS) $(EXECUTABLE) mandelbrot.png
