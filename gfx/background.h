class background{
    friend class Sprite;
    
    private:
        bool loaded;
        SDL_Rect size;
        SDL_Rect temp;
        SDL_Texture *Texture;
        SDL_Renderer *randr;
        engine *ref;
    public:
        background(const char* file, engine EngineRef);
        void draw(void);
        void destroy(void);
};


void background::draw(void){
    

    if(loaded){
        SDL_RenderCopy(randr, Texture, NULL, &temp);
    } else {
        printf("Attempted to draw a background that is not yet loaded");
    }
}



background::background(const char* file, engine EngineRef){

    // Reset flag
    loaded = false;
    
    
    // Copy renderer reference
    randr = EngineRef.renderer;
    
    // Load picture
    SDL_Surface *image_surface = IMG_Load(file);
    
    // Create a new surface for temporary application
    SDL_Surface *temp_surface = SDL_CreateRGBSurface(0,EngineRef.get_width(),EngineRef.get_height(),32,0,0,0,0);  // for the looping and then texturing
    
    // Load picture
    if (image_surface != NULL){
        loaded = true;
    } else {
        printf("Failed at loading %s\n", file);
        exit(1);
    }
        
    // Original bg size
    size.x=0;
    size.y=0;
    size.h=image_surface->h;
    size.w=image_surface->w;   
    
    // SDL_Rect used for moving the surface around and apply it
    SDL_Rect pos;
    pos.x=0;
    pos.y=0;
    pos.h=image_surface->h;
    pos.w=image_surface->w;   
      
    // Multipliers for height and width. Used in the loop below to apply 
    // the surface multiple times to help loop it
    uint8_t height_mult=0;
    uint8_t width_mult=0;
    
    // loop surface
    while ( (height_mult*image_surface->h) < EngineRef.get_height() ){
    
        // Reset width for every line
        width_mult=0;
        
        // Set new line we're at using the multiplier
        pos.y=(height_mult*image_surface->h);
        
        // Create all horizontal copies
        while (  width_mult*image_surface->w < EngineRef.get_width() ){
        
            pos.x=(width_mult*image_surface->w);
            SDL_BlitSurface(image_surface, &size, temp_surface, &pos);
            
            width_mult++;
        }
                     
        height_mult++;
    }
    
    // Create the texture
    Texture = SDL_CreateTextureFromSurface(EngineRef.renderer, temp_surface);

    
    // Get Reference to engine obj
    ref = &EngineRef;
    
    temp.x=0;
    temp.y=0;
    temp.w=ref->get_width();
    temp.h=ref->get_height();
    
    // Clear surfaces from memory
    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(temp_surface);
    
    
}


void background::destroy(void){
    SDL_DestroyTexture(Texture);
}

