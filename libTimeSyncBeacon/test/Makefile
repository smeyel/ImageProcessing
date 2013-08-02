CC = g++
AR = ar

OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LIBS = `pkg-config --libs opencv`

CFLAGS = -c -Wall -I ../include $(OPENCV_CFLAGS)
LDFLAGS = $(OPENCV_LIBS)

all: testConverter testReader


TESTCONVERTER_OBJECTS = test_TimeSyncBeaconConverter.o ../libTimeSyncBeacon.a
TESTREADER_OBJECTS =  test_TimeSyncBeaconReader.o ../libTimeSyncBeacon.a


testConverter: $(TESTCONVERTER_OBJECTS)
	$(CC) -o testConverter $(TESTCONVERTER_OBJECTS) $(LDFLAGS)

testReader: $(TESTREADER_OBJECTS)
	$(CC) -o testReader $(TESTREADER_OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o testConverter testReader
	
.PHONY: all clean
