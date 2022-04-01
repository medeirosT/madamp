#include <fcntl.h>
#include "sys/ioctl.h"

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

class display{
	private:
		char brightness_file[128];
		uint8_t default_brightness;
		uint8_t brightness;
		bool disabled;
		bool sleeping;
		bool fading;
		uint32_t start_timestamp;
		joystick *joy           = NULL;         	// Reference to the input
	public:
		uint8_t get_brightness(void);
		bool set_brightness(uint8_t new_brightness);
		display( const char *brightness_file_location, joystick *newjoy);
		void destroy(void);
		void loop(void);

};

#define MS_UNTIL_OFF 2000


display::display( const char *brightness_file_location, joystick *newjoy){
	strcpy(brightness_file, brightness_file_location);
	
	default_brightness 	= get_brightness();
	brightness			= default_brightness;
	joy     			= newjoy;
	start_timestamp 	= SDL_GetTicks();
	sleeping			= false;
	fading				= false;
	disabled			= false;
}



void display::loop(void){
	if (joy->any){
		start_timestamp = SDL_GetTicks();
		set_brightness(default_brightness);
		sleeping = false;
	} else if (sleeping == false && SDL_GetTicks() - start_timestamp > MS_UNTIL_OFF ){
		set_brightness(0);	
		sleeping = true;	
	}
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

void display::destroy(void){
	set_brightness(default_brightness);
	
}


bool display::set_brightness(uint8_t new_brightness){
	if (disabled) return false;
	if (brightness == new_brightness) return false;
	FILE *fp = fopen(brightness_file, "w");
	

	if (fp == NULL){
		printf("Unable to open brightness file for writing, giving up!\n");
		return false;
	}
	
	char temp[4];
	sprintf(temp, "%d", new_brightness);
	fputs(temp, fp);
	
	brightness = new_brightness;
	
	
	fclose(fp);
	return true;
}