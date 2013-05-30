CC = g++
AR = ar

CFLAGS = -c -Wall -I include/
LDFLAGS = 

all: libTimeSyncBeacon.a


LIBTIMESYNCBEACON_OBJECTS =  


libTimeSyncBeacon.a: $(LIBTIMESYNCBEACON_OBJECTS)
	$(AR) rcs libTimeSyncBeacon.a $(LIBTIMESYNCBEACON_OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -rf src/*.o libTimeSyncBeacon.a
	
.PHONY: all clean
