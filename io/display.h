#include <fcntl.h>
#include "sys/ioctl.h"

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

class display{
	
		char brightness_file[128];
		uint8_t brightness;
		bool disabled;
		bool fading;
	
	public:
		uint8_t get_brightness(void);
		bool set_brightness(uint8_t new_brightness);
		display( const char *brightness_file_location);

};

display::display( const char *brightness_file_location){
	strcpy(brightness_file, brightness_file_location);
	brightness = get_brightness();

}


uint8_t display::get_brightness(void){
	if (disabled) return 0;
	int tempbrightness;
	FILE *fp = fopen(brightness_file, "r");
	
	if (fp == NULL){
		printf("Error opening brightness file. Disabling feature\n");
		disabled = true;
		return 0;
	} else {
		char bright[5];
		if (fgets(bright,4,fp) == NULL){
			printf("Error reading brightness value. Disabling feature\n");
			disabled = true;
			return 0;
		}
		tempbrightness = atoi(bright);
		printf("Brightness detected : %d\n", tempbrightness);
	
		fclose(fp);
	}
	
	if (tempbrightness > 100 || 0 > tempbrightness ){
		printf("Odd value obtained, assuming wrong device and disabling feature\n");
		disabled = true;
		return 0;
	}
	
	return tempbrightness;
}




bool display::set_brightness(uint8_t new_brightness){
	if (disabled) return false;
	
	FILE *fp = fopen(brightness_file, "w");
	if (fp == NULL){
		printf("Unable to open brightness file for writing, giving up!\n");
		return false;
	}
	
	
	
	return true;
}