class font{
    private:
        TTF_Font *fnt;
        SDL_Color color;
        engine *ref;
        SDL_Rect pos;
        SDL_Texture *texture;
        char old_text[256];
        int font_size;
        bool changed;               // will onyl redraw if set to true
        bool rendered_once;
    public:
        font(const char* file, int size, engine EngineRef);
        void draw(const char* text);
        void draw_to_surface(const char* text, SDL_Surface *target);
        void setX(int newx);
        void setY(int newy);
        void setRGB(uint8_t r,uint8_t g,uint8_t b);
        void setRGBSDL(SDL_Color *newcolor);
};



font::font(const char* file, int size, engine EngineRef){
    if ( TTF_Init() == -1){
        EngineRef.debug_message("Error attempting to init TTF\n");
        exit(1);
    }
    fnt = TTF_OpenFont( file, size);
    if (fnt == NULL){
        EngineRef.debug_message("Error loading TTF file\n");
        exit(1);
    }
    ref = &EngineRef;
    
    pos.x = 0;
    pos.y = 0;
    
    font_size=size;
    
    color = {255,255,255,0};
    changed = true;
    rendered_once = false;
}

void font::setX(int newx){
    pos.x = newx;
    changed = true;
}

void font::setY(int newy){
    pos.y = newy;
    changed = true;
}

void font::draw(const char* text){

    if (changed || strcmp(text, old_text) == 0){
    
        // Reset status so we don't keep re-drawing
        changed = false;
        
        // copy old text
        sprintf(old_text,"%s", text);
       
        if (rendered_once){
        
            // Destroy the texture to avoid a memory leak
            SDL_DestroyTexture(texture);
            
            // Set it to NULL to be *Absolutely sure*
            texture = NULL;
        }
        
        // Create new temporary surface
        SDL_Surface *message = NULL;
        
        // Render font
        message = TTF_RenderText_Solid(fnt,text,color);
        
        // Create a texture from that surface
        texture = SDL_CreateTextureFromSurface(ref->renderer, message);
        
        // Update SDL_Rect for rendering
        pos.w=message->w;
        pos.h=message->h;
        
        // Clear the surface from memory
        SDL_FreeSurface(message);
        
        // To let us know we rendered at least once. 
        // Fixes segmentation fault on deleting old texture when
        // it didn't exist or was even created once.
        rendered_once = true;
    }
    
    // Render from memory 
    SDL_RenderCopy(ref->renderer, texture, NULL, &pos);

}

void font::setRGB(uint8_t r,uint8_t g,uint8_t b){
    color = {r,g,b,0};
    changed = true;
}

void font::setRGBSDL(SDL_Color *newcolor){
    setRGB(newcolor->r, newcolor->g, newcolor->b);
}

void font::draw_to_surface(const char* text, SDL_Surface *target){
    // if we're calling this function, no need to check.
    // if we already rendered. This is for a surface, so...
    
    SDL_Surface *temp;
    temp = TTF_RenderText_Blended(fnt,text,color);
    SDL_BlitSurface(temp, NULL, target, &pos);
	SDL_FreeSurface(temp);

}
