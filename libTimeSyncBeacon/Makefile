CC = g++
AR = ar

OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LIBS = `pkg-config --libs opencv`

override CXXFLAGS += -c -Wall -I include/ $(OPENCV_CFLAGS)
LDFLAGS = 

all: libTimeSyncBeacon.a


LIBTIMESYNCBEACON_OBJECTS = src/TimeSyncBeaconConverter.o src/SmartVector.o src/CircleXYR.o src/TimeSyncBeaconReader.o


libTimeSyncBeacon.a: $(LIBTIMESYNCBEACON_OBJECTS)
	$(AR) rcs libTimeSyncBeacon.a $(LIBTIMESYNCBEACON_OBJECTS)

.cpp.o:
	$(CC) $(CXXFLAGS) $< -o $@
	
test: libTimeSyncBeacon.a
	make -C test all
	
clean:
	rm -rf src/*.o libTimeSyncBeacon.a
	make -C test clean
	
.PHONY: all clean test
