all: libTwoColorCircleMarker
 
libTwoColorCircleMarker:
	make -C libTwoColorCircleMarker/
	
libTimeSyncBeacon:
	make -C libTimeSyncBeacon/

clean:
	make -C libTwoColorCircleMarker/ clean
	
.PHONY: all clean libTwoColorCircleMarker libTimeSyncBeacon