#ifndef TIMESYNCBEACONCONVERTER_H_
#define TIMESYNCBEACONCONVERTER_H_

#include <stdint.h>



class TimeSyncBeaconConverter{

public:

	TimeSyncBeaconConverter();

	TimeSyncBeaconConverter( uint8_t brightness_max,
	                         uint8_t run_threshold,
	                         uint8_t gray_low_threshold,
	                         uint8_t gray_high_threshold,
	                         uint8_t edge_threshold);

	void SetBrightnessMax(uint8_t brightness_max);
	void SetRunThreshold(uint8_t run_threshold);
	void SetGrayThreshold( uint8_t gray_low_threshold,
	                       uint8_t gray_high_threshold);
	void SetEdgeThreshold(uint8_t edge_threshold);

	uint8_t GetBrightnessMax();
	uint8_t GetRunThreshold();
	uint8_t GetGrayLowThreshold();
	uint8_t GetGrayHighThreshold();
	uint8_t GetEdgeThreshold();

	uint32_t GetTimeMsFromBrightness(const uint8_t brightness[64]);

private:

	uint8_t brightness_max;
	uint8_t edge_threshold;
	uint8_t run_threshold;
	uint8_t gray_low_threshold;
	uint8_t gray_high_threshold;

	uint32_t ERROR(const char* str);
	uint32_t RET(const char* str,  uint32_t ms);

	uint32_t binAndRunFirstToMs(uint32_t bin, int run_first);

	uint32_t binaryToGray(uint32_t num);
	uint32_t grayToBinary(uint32_t num);

};

#endif //TIMESYNCBEACONCONVERTER_H_

