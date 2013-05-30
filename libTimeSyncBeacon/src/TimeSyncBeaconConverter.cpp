
#include "TimeSyncBeaconConverter.h"

#define DEBUG	1

#if DEBUG
#include <iostream>
#include <iomanip>
using namespace std;
#endif



TimeSyncBeaconConverter::TimeSyncBeaconConverter(){
	brightness_max =      100;
	run_threshold =        15;
	gray_low_threshold =   30;
	gray_high_threshold =  70;
	edge_threshold =       80;
}
	
TimeSyncBeaconConverter::TimeSyncBeaconConverter( uint8_t brightness_max,
                                                  uint8_t run_threshold,
                                                  uint8_t gray_low_threshold,
                                                  uint8_t gray_high_threshold,
                                                  uint8_t edge_threshold){
	this->brightness_max = brightness_max;
	this->run_threshold = run_threshold;
	this->gray_low_threshold = gray_low_threshold;
	this->gray_high_threshold = gray_high_threshold;
	this->edge_threshold = edge_threshold;
}

void TimeSyncBeaconConverter::SetBrightnessMax(uint8_t brightness_max){
	this->brightness_max = brightness_max;
}
	
void TimeSyncBeaconConverter::SetRunThreshold(uint8_t run_threshold){
	this->run_threshold = run_threshold;
}

void TimeSyncBeaconConverter::SetGrayThreshold(uint8_t gray_low_threshold, uint8_t gray_high_threshold){
	this->gray_low_threshold = gray_low_threshold;
	this->gray_high_threshold = gray_high_threshold;
}

void TimeSyncBeaconConverter::SetEdgeThreshold(uint8_t edge_threshold){
	this->edge_threshold = edge_threshold;
}

uint8_t TimeSyncBeaconConverter::GetBrightnessMax(){
	return brightness_max;
}

uint8_t TimeSyncBeaconConverter::GetRunThreshold(){
	return run_threshold;
}

uint8_t TimeSyncBeaconConverter::GetGrayLowThreshold(){
	return gray_low_threshold;
}

uint8_t TimeSyncBeaconConverter::GetGrayHighThreshold(){
	return gray_high_threshold;
}

uint8_t TimeSyncBeaconConverter::GetEdgeThreshold(){
	return edge_threshold;
}

uint32_t TimeSyncBeaconConverter::GetTimeMsFromBrightness(const uint8_t brightness[64]){

	#if DEBUG
	cout << "brightness = ";
	for(int i=64 ; i-->0 ; ){
		if(i==47 || i==31 || i==15)
			cout << "             ";
		cout << setw(4) << (int)brightness[i];
		if(i==48 || i==32 || i==16 || i==0)
			cout << endl;
	}
	cout << endl;
	#endif
	
	#if DEBUG
	cout << "brightness_max =      " << setw(3) << (int)brightness_max      << endl;
	cout << "run_threshold =       " << setw(3) << (int)run_threshold       << endl;
	cout << "gray_low_threshold =  " << setw(3) << (int)gray_low_threshold  << endl;
	cout << "gray_high_threshold = " << setw(3) << (int)gray_high_threshold << endl;
	cout << "edge_threshold =      " << setw(3) << (int)edge_threshold      << endl;
	cout << endl;
	#endif
	
	//led states
	const int unstable = -1;
	const int off = 0;
	const int on = 1;
	
	int run[30];            //run led states
	int gray[30];           //gray led states
	int gray_unstable = -1; //index of the unstable led in gray
	int run_change = 0;     //count of the changes in run
	int run_length = 0;     //count of the on states in run

	//i:  index in brightness
	//cr: count run
	//cg: count gray
	for(int i=0, cr=0, cg=0 ; i<64 ; i++){
	
		if(brightness[i] > brightness_max)
			return ERROR("too high brightness value");
	
		//edge
		if(i== 0 || i==15 || i==48 || i==63){
			if(brightness[i] < edge_threshold)
				return ERROR("edge count < 4");
		}
		
		//run
		else if(i < 32){
			if(brightness[i] < run_threshold)
				run[cr] = off;
			else{
				run[cr] = on;
				run_length++;
			}
			if((cr>0) && (run[cr] != run[cr-1]))
				run_change++;
			cr++;
		}
		
		//gray
		else{
			if(brightness[i] < gray_low_threshold)
				gray[cg++] = off;
			else if(brightness[i] < gray_high_threshold){
				if(gray_unstable != -1)
					return ERROR("unstable count > 1");
				gray_unstable = cg;
				gray[cg++] = unstable;
			}
			else
				gray[cg++] = on;
		}
		
	}



	#if DEBUG
	cout << "gray =  ";
	for(int i=30 ; i-->0 ; ){
		if(i==15)
			cout << "       ";
		switch(gray[i]){
			case -1: cout << "?"; break;
			case  0: cout << "0"; break;
			case +1: cout << "1"; break;
		}
		if(i==16 || i==0)
			cout << endl;
	}
	cout << "run =  ";
	for(int i=30 ; i-->0 ; ){
		if(i==13)
			cout << "        ";
		switch(run[i]){
			case -1: cout << "?"; break;
			case  0: cout << "0"; break;
			case +1: cout << "1"; break;
		}
		if(i==14 || i==0)
			cout << endl;
	}
	cout << endl;
	#endif



	#if DEBUG
	cout << "run_change = " << run_change << endl;
	#endif

	//0000000000000000   0   ERROR
	//0000001111110000   2   OK
	//0000000000111111   1   OK
	//1111000000000011   2   OK
	//1111111111111111   0   ERROR
	//0000011100000111   3   ERROR
	if(run_change == 0)
		return ERROR("run is full of the same bits");
	if(run_change > 2)
		return ERROR("run is discontinuous");

	#if DEBUG
	cout << "run_length = " << run_length << endl;
	#endif



	//turn in run
	bool turn = run[29] && run[0];

	#if DEBUG
	cout << "turn = " << turn << endl;
	#endif



	//run first bit, beginning of exposure
	int run_first = -1; //index of the first on state in run (first in time)
	if(!turn){
		//00000011111X0000
		//000000000011111X
		for(int i=0 ; run_first==-1 && i<30 ; i++)
			if(run[i])
				run_first = i;
	}
	else{
		//111X000000000011
		for(int i=30 ; run_first==-1 && i-->0 ; )
			if(!run[i])
				run_first = i+1;
	}

	#if DEBUG
	cout << "run_first = " << run_first << endl;
	cout << endl;
	#endif
				


	uint32_t K = 0; //gray code in uint32_t, unstable => off (unstable led is handled as 0)
	for(int i=0 ; i<30 ; i++)
		if(gray[i] == on)
			K |= (uint32_t)0x01 << i;
	uint32_t Kb = grayToBinary(K); //code in binary format


	
	#if DEBUG
	cout << "gray_unstable = " << gray_unstable << endl;
	cout << "K = " << K << ((gray_unstable!=-1) ? " (? = 0)" : "") << endl;	//unstable => off (unstable led is handled as 0)
	cout << "Kb = " << Kb << endl;
	cout << endl;
	#endif

	

	//no turn in run
	if(!turn){
		if(gray_unstable != -1)
			return ERROR("no turn in run, but gray has unstable");
		return RET("no turn in run, and gray has no unstable", binAndRunFirstToMs(Kb, run_first));
	}
	
	//turn in run
	else{
	
		int run_high_count = 30 - run_first;             //continuous on states in run in the high bits, XXXX000000000011
		int run_low_count = run_length - run_high_count; //continuous on states in run in the low bits,  11110000000000XX
		
		#if DEBUG
		cout << "run_high_count = " << setw(2) << run_high_count << endl;
		cout << "run_low_count =  " << setw(2) << run_low_count << endl;
		cout << endl;
		#endif

		//high dominant => gray is the beginning of the exposure
		if(run_high_count >= 2*run_low_count){
			if(gray_unstable != -1)
				return ERROR("turn and dominant in run (high), but gray has unstable");
			return RET("turn and dominant in run (high), and gray has no unstable, K=begin", binAndRunFirstToMs(Kb, run_first));
		}

		//low dominant => gray is the end of the exposure
		else if(run_low_count >= 2*run_high_count){
			if(gray_unstable != -1)
				return ERROR("turn and dominant in run (low), but gray has unstable");
			return RET("turn and dominant in run (low), and gray has no unstable, K=end", binAndRunFirstToMs(Kb-1, run_first));
		}

		//no dominant
		else{
			if(gray_unstable == -1)
				return ERROR("turn but no dominant in run, but gray has no unstable (OK?)");
			uint32_t K_p = binaryToGray(Kb+1);             //binary code +1 in gray code
			uint32_t K_m = binaryToGray(Kb-1);             //binary code -1 in gray code
			uint32_t x_p = K_p ^ K;                        //bit mask of the changing bit in K => K_p
			uint32_t x_m = K_m ^ K;                        //bit mask of the changing bit in K_m => K
			uint32_t gu = (uint32_t)0x01 << gray_unstable; //bit mask of unstable bit in gray
			
			#if DEBUG
			cout << setbase(16);
			cout << setfill('0');
			cout << "K =   0x" << setw(8) << K << endl;
			cout << "K_p = 0x" << setw(8) << K_p << endl;
			cout << "x_p = 0x" << setw(8) << x_p << endl;
			cout << endl;
			cout << "K =   0x" << setw(8) << K << endl;
			cout << "K_m = 0x" << setw(8) << K_m << endl;
			cout << "x_m = 0x" << setw(8) << x_m << endl;
			cout << endl;
			cout << "gu =  0x" << setw(8) << gu << endl;
			cout << endl;
			cout << setbase(10);
			#endif

			if(x_p & gu)
				return RET("turn but no dominant in run, gray has the +1 unstable, K=begin", binAndRunFirstToMs(Kb, run_first));
			else if(x_m & gu)
				return RET("turn but no dominant in run, gray has the -1 unstable, K=end", binAndRunFirstToMs(Kb-1, run_first));
			else
				return ERROR("turn but no dominant in run, gray has unstable, but no +1 or -1 bit");
		}
		
	}

}


uint32_t TimeSyncBeaconConverter::ERROR(const char* str){
	#if DEBUG
	cout << "ERROR: " << "\"" << str << "\"" << " => " << 0 << endl;
	#else
	//TODO: exception?
	#endif
	return 0;
}

uint32_t TimeSyncBeaconConverter::RET(const char* str,  uint32_t ms){
	#if DEBUG
	cout << "RET: " << "\"" << str << "\"" << " => " << ms << endl;
	#endif
	return ms;
}

uint32_t TimeSyncBeaconConverter::binAndRunFirstToMs(uint32_t bin, int run_first){
	uint32_t count = 30*bin + (uint32_t)run_first;
	#if DEBUG
	return count;
	#else
	return count * 1000 / 1024;	//one step is (1000/1024)ms
	#endif
}

uint32_t TimeSyncBeaconConverter::binaryToGray(uint32_t num){
	return (num >> 1) ^ num;
}
 
uint32_t TimeSyncBeaconConverter::grayToBinary(uint32_t num){
	uint32_t mask;
	for(mask = num >> 1; mask != 0; mask = mask >> 1)
		num = num ^ mask;
	return num;
}

