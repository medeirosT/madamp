class theme{
    private:
        char path[128];
        char ini_path[150];
    public:
        theme(const char* name);
        char file_bg[150];
        char file_font[150];
        char cursor[2];
        int font_size;
        SDL_Color font_color;
        SDL_Color status_bg_color;
        SDL_Color status_font_color;
        SDL_Color progress_bar_bg_color;

};


theme::theme(const char* name){
    if (name == NULL){
        printf("Couldn't find `theme` value in main config.ini. Attempting to load `default` theme");
        sprintf(path, "themes/default/");
    } else {
        sprintf( path, "themes/%s/", name );
    }
    
    
    sprintf(ini_path,"%stheme.ini", path);   
    
    // Open the theme config
    config cfg(ini_path);
    
    
    // Load values
    sprintf(file_bg     	,"%s%s", path, cfg.get_string_by_key("background"));
    sprintf(file_font   	,"%s%s", path, cfg.get_string_by_key("font"));
    sprintf(cursor     	 	,"%s", cfg.get_string_by_key("cursor"));
    font_size           	= cfg.get_int_by_key("fontsize",8);
    
    
    font_color          	= { (uint8_t)cfg.get_int_by_key("fontr",180),(uint8_t)cfg.get_int_by_key("fontg",180),
    							(uint8_t)cfg.get_int_by_key("fontb",180),0};
    							
    status_bg_color     	= { (uint8_t)cfg.get_int_by_key("statusr",32),(uint8_t)cfg.get_int_by_key("statusg",32),
                            	(uint8_t)cfg.get_int_by_key("statusb",32),0};
                            	
    status_font_color   	= { (uint8_t)cfg.get_int_by_key("statusfontr",180),(uint8_t)cfg.get_int_by_key("statusfontg",180),
                            	(uint8_t)cfg.get_int_by_key("statusfontb",180),0};
                            	
    progress_bar_bg_color = { 	(uint8_t)cfg.get_int_by_key("progressr",128), (uint8_t)cfg.get_int_by_key("progressg",0),
    							(uint8_t)cfg.get_int_by_key("progressb",0), 0 };
    
    
}


