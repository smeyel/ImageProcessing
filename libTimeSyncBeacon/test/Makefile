CC = g++
AR = ar

OPENCV_CFLAGS = `pkg-config --cflags opencv`
OPENCV_LIBS = `pkg-config --libs opencv`

CFLAGS = -c -Wall -I ../include $(OPENCV_CFLAGS)
LDFLAGS = $(OPENCV_LIBS)

all: test


TEST_OBJECTS =  main.o


test: $(TEST_OBJECTS)
	$(CC) -o test $(TEST_OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o test
	
.PHONY: all clean
