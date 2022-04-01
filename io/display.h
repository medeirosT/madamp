#include <fcntl.h>
#include "sys/ioctl.h"

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

class display{
	private:
		char brightness_file[128];
		uint8_t default_brightness;
		uint8_t brightness;
		uint8_t timeout_in_seconds;
		bool disabled;
		bool sleeping;
		bool fading;
		bool force_val;
		uint32_t start_timestamp;
		joystick *joy           = NULL;         	// Reference to the input
		config *cfg             = NULL;         	// Reference to the config
	public:
		uint8_t get_brightness(void);
		bool set_brightness(uint8_t new_brightness);
		display( const char *brightness_file_location, joystick *newjoy,config *maincfg);
		void destroy(void);
		void loop(void);

};




display::display( const char *brightness_file_location, joystick *newjoy,config *maincfg){
	strcpy(brightness_file, brightness_file_location);
	
	default_brightness 	= get_brightness();
	brightness			= default_brightness;
	joy     			= newjoy;
	cfg					= maincfg;
	start_timestamp 	= SDL_GetTicks();
	sleeping			= false;
	fading				= false;
	disabled			= false;
	force_val			= cfg->get_int_by_key("forcebacklight", 0) == 1;
	timeout_in_seconds	= cfg->get_int_by_key("backlighttimeout", 15);
	
}



void display::loop(void){
	if (joy->any){
		start_timestamp = SDL_GetTicks();
		set_brightness(default_brightness);
		sleeping = false;
	} else if ( sleeping == false && SDL_GetTicks() - start_timestamp > (timeout_in_seconds*1000)){
		set_brightness(0);	
		sleeping = true;	
	}
}


uint8_t display::get_brightness(void){
	if (disabled && false == force_val) return 0;
	if (force_val) return cfg->get_int_by_key("forcebacklightvalue",30);
	
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