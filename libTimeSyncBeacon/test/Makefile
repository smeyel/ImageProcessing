CC = g++
AR = ar

OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LIBS = `pkg-config --libs opencv`

CFLAGS = -c -Wall -I ../include $(OPENCV_CFLAGS)
LDFLAGS = $(OPENCV_LIBS)

all: testConverter testReader


TESTCONVERTER_OBJECTS = test_TimeSyncBeaconConverter.o ../libTimeSyncBeacon.a
TEST_OBJECTS =  test_TimeSyncBeaconReader.o


testConverter: $(TESTCONVERTER_OBJECTS)
	$(CC) -o testConverter $(TESTCONVERTER_OBJECTS) $(LDFLAGS)

testReader: $(TEST_OBJECTS)
	$(CC) -o testReader $(TEST_OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o testConverter testReader
	
.PHONY: all clean
