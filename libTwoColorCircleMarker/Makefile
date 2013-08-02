CC = g++
AR = ar
OPENCV_CFLAGS = `pkg-config --cflags opencv`
LIBLOGCONFIGTIME_PATH = ../../Framework/libLogConfigTime

CFLAGS = -c -Wall -I include/ -I $(LIBLOGCONFIGTIME_PATH)/include $(OPENCV_CFLAGS)
LDFLAGS = 

all: libTwoColorCircleMarker.a


LIBTWOCOLORCIRCLEMARKER_OBJECTS =  src/FastColorFilter.o src/MarkerCC2.o src/MarkerCC2Locator.o src/MarkerCC2Tracker.o src/TwoColorLocator.o


libTwoColorCircleMarker.a: $(LIBTWOCOLORCIRCLEMARKER_OBJECTS)
	$(AR) rcs libTwoColorCircleMarker.a $(LIBTWOCOLORCIRCLEMARKER_OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf src/*.o libTwoColorCircleMarker.a
	
.PHONY: all clean
