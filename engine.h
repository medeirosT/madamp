class engine{
    private:
        // Screen width and height
        unsigned short int SCREEN_WIDTH, SCREEN_HEIGHT;
        SDL_Window *window;
        
        bool gfx_init;
        bool sfx_init;
        
        // Used for calculation of FPS in current second, do not use this!
        uint16_t fps;

        // Used to store result for FPS in last second. Use this one!
        uint16_t fps_count;

        // Number of FPS we're limiting to, if at all
        uint8_t fps_limit;

        // How many milliseconds between frames
        uint8_t frame_interval;
        
        // When did we last draw a frame?
        uint32_t last_drawn_frame_timestamp;
        
        void calculate_fps(void);
    public:
        engine(int width, int height, uint8_t max_fps);
        bool init_gfx(void);
        bool init_sfx(void);
        bool clear_screen(void);
        void destroy(void);
        void flush_screen(void);
        void disable_debug_mode(void);
        SDL_Renderer *renderer;
        void loop(void);
        void draw_square(int x1, int y1, int x2, int y2, SDL_Color *color);
        unsigned short int get_height(void);
        unsigned short int get_width(void);
        
};




/**
*   Initializer for engine object
*   @param {integer} width - screen width to initialize with
*   @param {integer} height - screen height to initialize with
*/
engine::engine( int width, int height, uint8_t max_fps ){
	// The RG devices are rotated, this fixes the sizing on PC
	#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    SCREEN_WIDTH = height;
    SCREEN_HEIGHT = width;
    #else
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    #endif
    
    window = NULL;
    renderer = NULL;
    gfx_init = false;
    fps=0;
    fps_count=0;
    last_drawn_frame_timestamp=0;
    fps_limit = max_fps;
    frame_interval = (max_fps == 0 ? 0 : (int)(1000 / fps_limit));
}

unsigned short int engine::get_height(void){
    return SCREEN_HEIGHT;
}

unsigned short int engine::get_width(void){
    return SCREEN_WIDTH;
}



bool engine::init_gfx(void){
    printf("Init Video\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { 
        printf( "SDL init failed.\n" ); 
        exit(2);
        return false;
    }
    
    printf("Creating Window\n");
    #if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
    window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_HEIGHT,SCREEN_WIDTH, 0);
    #else
    window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_HEIGHT,SCREEN_WIDTH, SDL_WINDOW_FULLSCREEN);
    #endif
    
    if (window == NULL){
        printf("Window generating failed\n");
        return false;
    }

    printf("Creating renderer\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL){
        printf("Failed to create renderer\n");
        return false;
    }
    
    printf("GFX init complete\n");
    gfx_init = true;
    
    SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_CRITICAL);
    return true;

}

void engine::flush_screen(void){
    if (gfx_init){
        SDL_RenderPresent(renderer);
    }

}

bool engine::clear_screen(void){
    if (gfx_init){
        SDL_RenderClear(renderer);
        return true;
    } 
    return false;

}


void engine::destroy(void){
    if (gfx_init){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }    
    if (sfx_init){
        Mix_Quit();
    }
    SDL_Quit();

}



/**
*	function basically calculates current framerate
*	detects when a second has passed and resets the counter
*	applies whatever value we had.
*/
void engine::calculate_fps(void){
	if( (int)SDL_GetTicks()/1000 > (int)last_drawn_frame_timestamp/1000 ){
		fps_count = fps;
		fps = 0;
	}
	fps++;
}


void engine::loop(void){
	// Calculate what tick are we drawing the next frame at
	uint32_t next_frame_timestamp = last_drawn_frame_timestamp+frame_interval;
	
	// Make sure the time we're supposed to draw at is upon us or passed:
	if (SDL_GetTicks() >= next_frame_timestamp){
	
		// Calculate FPS
		calculate_fps();
	
		// Make sure to get the new window surface and update it
		flush_screen();
	
		// Update last drawn timestamp
		last_drawn_frame_timestamp = SDL_GetTicks();

    } else {
 		// So for this function I'm calculating WHEN we should draw the next frame
 		// And using SDL_Delay to wait from now until then, to then draw the next frame.
		SDL_Delay( (next_frame_timestamp) - SDL_GetTicks() );
		
    }
}


void engine::draw_square(int x1, int y1, int x2, int y2, SDL_Color *color){
    SDL_Rect square;
    square.x = x1;
    square.y = y1;
    square.w = x2-x1;
    square.h = y2-y1;
    SDL_SetRenderDrawColor(renderer, color->r,color->g,color->b, color->a);
    SDL_RenderFillRect(renderer, &square);
}



bool engine::init_sfx(void){
    if (SDL_Init(SDL_INIT_AUDIO) < 0){
        printf("Unable to initialize audio\n");
        return false;
    }
   
    if (Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,128) == -1){
        printf("Unable to initialize SDL_Mixer\n");
        return false;
    }
    
    printf("Init Audio\n");
    sfx_init = true;
    return true;
}

