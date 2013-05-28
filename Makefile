all: libTwoColorCircleMarker
 
libTwoColorCircleMarker:
	make -C libTwoColorCircleMarker/
	
clean:
	make -C libTwoColorCircleMarker/ clean
	
.PHONY: all clean libTwoColorCircleMarker