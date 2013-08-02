all: libTwoColorCircleMarker libTimeSyncBeacon
 
libTwoColorCircleMarker:
	make -C libTwoColorCircleMarker/
	
libTimeSyncBeacon:
	make -C libTimeSyncBeacon/

clean:
	make -C libTwoColorCircleMarker/ clean
	make -C libTimeSyncBeacon/ clean
	
.PHONY: all clean libTwoColorCircleMarker libTimeSyncBeacon