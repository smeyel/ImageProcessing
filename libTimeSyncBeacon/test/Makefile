CC = g++
AR = ar

OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LIBS = `pkg-config --libs opencv`

CFLAGS = -c -Wall -I ../include $(OPENCV_CFLAGS)
LDFLAGS = $(OPENCV_LIBS)

all: testConverter test


TESTCONVERTER_OBJECTS = test_TimeSyncBeaconConverter.o ../libTimeSyncBeacon.a
TEST_OBJECTS =  main.o


testConverter: $(TESTCONVERTER_OBJECTS)
	$(CC) -o testConverter $(TESTCONVERTER_OBJECTS) $(LDFLAGS)

test: $(TEST_OBJECTS)
	$(CC) -o test $(TEST_OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o testConverter test
	
.PHONY: all clean
