CC = g++
AR = ar

CFLAGS = -c -Wall -I include/
LDFLAGS = 

all: libTimeSyncBeacon.a


LIBTIMESYNCBEACON_OBJECTS = src/TimeSyncBeaconConverter.o


libTimeSyncBeacon.a: $(LIBTIMESYNCBEACON_OBJECTS)
	$(AR) rcs libTimeSyncBeacon.a $(LIBTIMESYNCBEACON_OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf src/*.o libTimeSyncBeacon.a
	make -C test clean
	
.PHONY: all clean
