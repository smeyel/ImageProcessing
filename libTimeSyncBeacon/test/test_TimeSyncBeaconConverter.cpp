
#include "TimeSyncBeaconConverter.h"
#include <stdint.h>

uint8_t brightness[64] = {
	100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 100, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  55,  88,   0,   0, 
	 88,  88,  88,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  88,  88,  88, 100, 
};


int main(void){

	//reverse the brightness array,
	//because the bottom right is LED0,
	//but above the bottom right element has index 63
	for(int i=0 ; i<32 ; i++){
		uint8_t tmp = brightness[i];
		brightness[i] = brightness[63-i];
		brightness[63-i] = tmp;
	}

	TimeSyncBeaconConverter c;
	c.SetBrightnessMax(100);
	c.SetRunThreshold(15);
	c.SetGrayThreshold(30, 70);
	c.SetEdgeThreshold(80);
	c.GetTimeMsFromBrightness(brightness);

	return  0;

}
