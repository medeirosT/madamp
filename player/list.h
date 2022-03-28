#include <taglib/tag_c.h>
#define TAGLIB_STATIC 

class list{
    private:
        engine *ref             = NULL;         	// Reference to the engine
        font *fnt               = NULL;         	// Reference to the font
        config *cfg             = NULL;         	// Reference to the config
        theme *ui               = NULL;         	// Reference to the theme config
        joystick *joy           = NULL;         	// Reference to the input
        directory *files        = NULL;         	// Reference to the List of files
        Mix_Music *song         = NULL;         	// Reference to the MIX class for SDL
        SDL_Texture *file_list  = NULL;         	// Texture for the file list
        SDL_Texture *cursor     = NULL;         	// Texture for the cursor
        SDL_Texture *status_bar_tex = NULL;     	// Texture for the status bar
        SDL_Texture *status_txt_tex = NULL;     	// Texture for the status bar text
        SDL_Texture *status_bar_progress_tex=NULL;	// Progress bar
        SDL_Rect cursor_pos;            	        // Position for the cursor
        SDL_Rect list_pos;              	        // position for the item list
        SDL_Rect status_bar;           	       	  	// position for the status bar
        SDL_Rect status_bar_progress;				// position for the status bar progress
        uint8_t cursor_index;                 	  	// Cursor index on the screen
        uint16_t playback_index;					// Index on the directory array of what we're playing
        char playing_filename[256];          		// filename we're playing
        char song_fullpath[1024];					// fullpath to the song we're playing
        bool button_down[6];               		    // whether or not the button was already checked as down so we don't double inputs;
        char status[256];               	        // text for the current status
        int start_timestamp;						// SDL_Ticks for when we started playing
        int last_drawn_progress_bar_timestamp;		// SDL Tick timestamp of the last time we drew the progress bar. We should be only doing it every 1s
        int song_length;							// length of song
        bool is_playing;					
        bool temp_message_displayed;				// Whether or not we're displaying a temporary message
        char oldstatus[256];
        uint32_t temp_message_displayed_timestamp;		// Time when we set the temp message;
        
        uint8_t playing_mode;						// 0 = normal, 1 = shuffle, 2 = repeat one, 3= play one
        
        void render_cursor_to_memory(void);
        void render_list_to_memory(void);
        void render_status_bar_to_memory(void);
        void render_progress_bar_to_memory(void);
        void play_song(void);
        void stop_song(void);
        void next_song(void);
        void update_progress_bar(void);
        void get_song_length(const char *song_fullpath);
        void set_new_status(const char *newstatus, bool temp);
    public:
        list(engine *engine, font *newfont, config *maincfg, theme *newtheme, joystick *newjoy, directory *file_list, Mix_Music *newsong);
        void render_to_memory(void);
        void draw(void);
        void loop(void);
        void destroy(void);
};


#define LINES_FOR_STATUS 			2                      	// Number of lines we set aside for like a status bar or something
#define MS_BETWEEN_PROGRESS_DRAWS 	500						// Number of milliseconds we have to wait until the progress bar needs to be redrawn
#define MS_FOR_TEMP_MESSAGE			1500					// Number of milliseconds a temp message can be displayed.


void list::destroy(void){
	ref           		  	= NULL;
    fnt               		= NULL;
    cfg             		= NULL;
    ui               		= NULL;
    joy          	 		= NULL;
    files        			= NULL;
    song         			= NULL;
    file_list  				= NULL;
    cursor     				= NULL;
    status_bar_tex 			= NULL;
    status_txt_tex 			= NULL;
    status_bar_progress_tex	= NULL;

}


list::list(engine *engine, font *newfont, config *maincfg, theme *newtheme, joystick *newjoy, directory *file_list, Mix_Music *newsong){

    ref     = engine;
    fnt     = newfont;
    cfg     = maincfg;
    ui      = newtheme;
    joy     = newjoy;
    files   = file_list;
    
    cursor_index=0;
    
    button_down[0] = false;     // down
    button_down[1] = false;     // up
    button_down[2] = false;     // A
    button_down[3] = false;     // B
    button_down[4] = false;     // X
    button_down[5] = false;		// R1
    
    status_bar.x = 0;
    status_bar.h = ui->font_size*LINES_FOR_STATUS;
    status_bar.y = cfg->get_int_by_key("screenheight",320)-status_bar.h;
    status_bar.w = cfg->get_int_by_key("screenwidth",480);
        
    sprintf(status,"Waiting for new file");
    
	//status_bar_progress
	status_bar_progress.x=0;
	status_bar_progress.w=0;
	status_bar_progress.h=status_bar.h*0.2;
	status_bar_progress.y=cfg->get_int_by_key("screenheight",480)-status_bar_progress.h;
    
    is_playing=false;
    
    playing_mode = cfg->get_int_by_key("playingmode",0);
    
    render_to_memory();
    
}


void list::draw(void){
    SDL_RenderCopy(ref->renderer, cursor, NULL, &cursor_pos);
    SDL_RenderCopy(ref->renderer, file_list, NULL, &list_pos);
    SDL_RenderCopy(ref->renderer, status_bar_tex, NULL, &status_bar);
	SDL_RenderCopy(ref->renderer, status_bar_progress_tex, NULL, &status_bar_progress);
}




void list::render_status_bar_to_memory(void){

    // Create a surface just for the box
    SDL_Surface *temp = SDL_CreateRGBSurface(0,status_bar.w,status_bar.h,16,0,0,0,0);
    SDL_FillRect(temp, NULL, SDL_MapRGB(temp->format, ui->status_bg_color.r,ui->status_bg_color.g,ui->status_bg_color.b));

    
    // Set the position of the font
    fnt->setX(ui->font_size);
    fnt->setY(ui->font_size/2);
    
    // Set the new color
    fnt->setRGBSDL(&ui->status_font_color);
    
    // Draw the font
    fnt->draw_to_surface(status, temp);
    
    // Reset font color
    fnt->setRGBSDL(&ui->font_color);
    
    // Make sure we destroy the old one
    SDL_DestroyTexture(status_bar_tex);
    
    // Set it to null for good measure.
    status_bar_tex = NULL;
    
    // Convert to texture
    status_bar_tex = SDL_CreateTextureFromSurface(ref->renderer, temp);
    
    SDL_FreeSurface(temp);

}

void list::render_to_memory(void){
	render_cursor_to_memory();
    render_list_to_memory();
    render_status_bar_to_memory();
    render_progress_bar_to_memory();
}


void list::set_new_status(const char *newstatus, bool temp = false){
	
	
	// Scenario list:
	// temp false, temp_message_displayed false			-- Just show whatever we got
	// temp false, temp_message_displayed true			-- Update oldstatus
	// temp true, temp_message_displayed false			-- update oldstatus, show what we got, set temp to true
	// temp true, temp_message_displayed true			-- show what we got, reset timer
	
	
	
	
	
	if ( false == temp && temp_message_displayed){
		strcpy(oldstatus,newstatus);
		return;
	} else if (temp && false == temp_message_displayed){
		strcpy(oldstatus,status);
		temp_message_displayed = true;
		temp_message_displayed_timestamp = SDL_GetTicks()+MS_FOR_TEMP_MESSAGE;
	} else if ( temp && temp_message_displayed ){
		temp_message_displayed_timestamp = SDL_GetTicks()+MS_FOR_TEMP_MESSAGE;	
	}
	
	

	if( strcmp(status, newstatus) != 0){
		sprintf(status,"%s", newstatus);
		render_status_bar_to_memory();
	}
}


void list::update_progress_bar(void){

	if ( 1 == Mix_PlayingMusic()){
		if (SDL_GetTicks() - last_drawn_progress_bar_timestamp < MS_BETWEEN_PROGRESS_DRAWS){
			return;
		}
		// Get where we are in the song.
		int cur = floor(SDL_GetTicks()/1000)-start_timestamp;
		
		
		int perc = (int)((cur*100)/song_length);
		
	
		last_drawn_progress_bar_timestamp = SDL_GetTicks();
		status_bar_progress.w=round(( cfg->get_int_by_key("screenwidth",480) * perc )/100);
	} else {
		
		if (is_playing){
			next_song();
		}
	}
}

void list::render_progress_bar_to_memory(void){
		
		if (status_bar_progress_tex != NULL){
			// Make sure we destroy the old one
			SDL_DestroyTexture(status_bar_progress_tex);

			// Set it to null for good measure.
			status_bar_progress_tex = NULL;
		}
		status_bar_progress_tex = SDL_CreateTexture(ref->renderer, 0, SDL_TEXTUREACCESS_TARGET, 1, 1);
		SDL_SetRenderTarget(ref->renderer, status_bar_progress_tex);
		SDL_SetRenderDrawColor(ref->renderer, ui->progress_bar_bg_color.r,ui->progress_bar_bg_color.g,ui->progress_bar_bg_color.b, 0x00);
		
		SDL_RenderDrawRect(ref->renderer,NULL);
		SDL_SetRenderDrawColor(ref->renderer, 0,0,0,0);
		SDL_SetRenderTarget(ref->renderer, NULL);
  

}




void list::render_cursor_to_memory(void){

    cursor_pos.x = 0;
    cursor_pos.y = ui->font_size * cursor_index;
    cursor_pos.h = ui->font_size;
    cursor_pos.w = ui->font_size*strlen(ui->cursor);
    
    fnt->setX(cursor_pos.x);
    fnt->setY(cursor_pos.y);
    
    SDL_Surface *temp = SDL_CreateRGBSurface(0,cursor_pos.w,cursor_pos.h,16,0,0,0,0);
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0, 0));
    
    fnt->draw_to_surface(ui->cursor, temp);
    
    cursor = SDL_CreateTextureFromSurface(ref->renderer, temp);
    
    SDL_FreeSurface(temp);
    

}




void list::render_list_to_memory(void){

    list_pos.y=0;
    list_pos.x=(strlen(ui->cursor) * ui->font_size);
    list_pos.w=(cfg->get_int_by_key("screenwidth",480)-list_pos.x);
    list_pos.h=cfg->get_int_by_key("screenheight",320)-(LINES_FOR_STATUS*ui->font_size);
    
    fnt->setY(0);
    fnt->setX(0);
    
    SDL_Surface *temp = SDL_CreateRGBSurface(0,list_pos.w,list_pos.h,16,0,0,0,0);
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0, 0));
    
    
    int i;
    for(i=0;i<files->length();i++){
        fnt->setY(i*ui->font_size);
        fnt->draw_to_surface(files->get_file(i), temp);
        
    }
    file_list = SDL_CreateTextureFromSurface(ref->renderer, temp);
    
    SDL_FreeSurface(temp);

}


void list::loop(void){
    int max_index = (int)(cfg->get_int_by_key("screenheight",320)/ui->font_size)-LINES_FOR_STATUS;
    max_index = ( max_index > files->length() ? files->length() : max_index);
    bool redraw_cursor = false;
    if (joy->down && button_down[0] == false){
        cursor_index = (cursor_index+1) % max_index;
        button_down[0] = true;
        redraw_cursor = true;
    }
    if (joy->up && button_down[1] == false){
        if (cursor_index == 0){
            cursor_index = max_index-1;
        } else {
            cursor_index--;
        }
        button_down[1]=true;
        redraw_cursor=true;
    }
    if (joy->a && button_down[2] == false){
    	playback_index = cursor_index;
        play_song();
        button_down[2]=true;
    } else if (joy->b && button_down[3] == false){
    	stop_song();
    	button_down[3]=true;
    } else if (joy->r1 && button_down[5] == false){
    	next_song();
    	button_down[5]=true;
    }
    
    if (joy->x && button_down[4] == false){
		//  	0 = normal, 1 = shuffle, 2 = repeat one, 3= play one
    	playing_mode = (playing_mode+1) % 4;
    	
    	switch(playing_mode){
    		case 0:
    			set_new_status("Normal",true);
    		break;
    		case 1:
    			set_new_status("Shuffle",true);
    		break;
    		case 2 :
    			set_new_status("Repeat One",true);
    		break;
    		case 3:
    			set_new_status("Play One",true);
    		break;
    		
    	}
    	button_down[4] = true;
    	cfg->set_int("playingmode",playing_mode);
    }
    
    button_down[0]=joy->down;
    button_down[1]=joy->up;
    button_down[2]=joy->a;
    button_down[3]=joy->b;
    button_down[4]=joy->x;
    button_down[5]=joy->r1;
    if (redraw_cursor){
        cursor_pos.y = ui->font_size * cursor_index;
        redraw_cursor=false;
    }
    
    
    update_progress_bar();
    if(temp_message_displayed && temp_message_displayed_timestamp <= SDL_GetTicks()){
    	temp_message_displayed = false;
    	temp_message_displayed_timestamp=0;
    	set_new_status(oldstatus);
    	
    }
     
}

void list::next_song(void){
	//  	0 = normal, 1 = shuffle, 2 = repeat one, 3= play one
	bool forcestop = false;
	uint16_t tempnum = playback_index;
	
	switch(playing_mode){
		case 0:
			if (playback_index+1 != files->length()){
				playback_index++;
			} else {
				playback_index=0;
			}
		break;
		case 1:
			
			if (files->length() > 1){
				while(tempnum == playback_index){
					playback_index = rand() % files->length();
				}
			}
			
		break;
		case 3:
			forcestop=true;
		break;
	
	} 
	
	if ( forcestop ){
		stop_song();
	} else if (is_playing){
		play_song();
	}
	
}


/**
*	This uses libTag to figure out a song's length.
*	@param {const char *} song_fullpath -- Full path of the file we're playing
*			so that libTag can read its info
*/
void list::get_song_length(const char *song_fullpath){

	TagLib_File *file;
	//TagLib_Tag *tag;
	
	// As well as properties
	const TagLib_AudioProperties *properties;
	
	// We're disabling unicode strings, was in the example, I'm keeping it.
	taglib_set_strings_unicode(false);	
	
	// Open the song file
	file = taglib_file_new(song_fullpath);
	
	// Make sure we are able to open it
	if(file == NULL)
  		return;

	// Grab all tags
	//tag = taglib_file_tag(file);
	
	// Grab all properties
	properties = taglib_file_audioproperties(file);
	
	// Get length in seconds
	song_length = taglib_audioproperties_length(properties);
	
	// Free all properties we made
	taglib_tag_free_strings();
	
	// Stop using the file.
	taglib_file_free(file);
}	


void list::stop_song(void){
	Mix_HaltMusic();
	Mix_FreeMusic(song);
	song = NULL;
	song_length = 0;
	start_timestamp=0;
	status_bar_progress.w = 0;
	last_drawn_progress_bar_timestamp = 0;
	is_playing=false;
	set_new_status("Waiting for new file");
	update_progress_bar();

}

void list::play_song(void){
	// First we make sure the file selected actually exists
    if (files->get_file(playback_index)!=NULL){

    	// Create a variable for the full path + filename so we can feed it to SDL_Mixer
        char fullpath[1024];

        // Join both variables onto fullpath
        sprintf(fullpath,"%s%s", cfg->get_string_by_key("musicdir"),files->get_file(playback_index));

        // Make sure we unload previous song
        stop_song();

        // Load the music file
        song = Mix_LoadMUS(fullpath);
        
        // And if it loads okay...
        if (song){
        	// we need to update the status, sooo
            char temp[256];
            sprintf(temp, "Currently playing : %s",files->get_file(playback_index));
            set_new_status(temp);

        	// Play the song
            Mix_PlayMusic(song, 0);

            // Get the song length using TagLib
            get_song_length(fullpath);
	
            if (song_length == 0){

            	start_timestamp=0;
            	last_drawn_progress_bar_timestamp=0;

            } else {

				// Make sure we keep track of when we started playing
				start_timestamp=floor(SDL_GetTicks()/1000);

				// and the last time we drew the progress bar
				last_drawn_progress_bar_timestamp=floor(SDL_GetTicks()/1000);

            
            }
			is_playing = true;
            
        }
    }

}
