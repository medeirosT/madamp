#ifndef _SPRITE_H
#define _SPRITE_H

#include "colorkey.h"

class Sprite {
   
    private:
        bool loaded;
        SDL_Rect size;
        SDL_Texture *Texture;
        SDL_Renderer *randr;
    public:
        Sprite(const char* file, int h, int w, SDL_Renderer* renderer, bool colorkey);
        void draw(void);
        void destroy(void);
        void setXY(int x, int y);
        int getY(void);
        int getX(void);
        SDL_Texture* get_texture(void);
};

Sprite::Sprite(const char* file, int h, int w, SDL_Renderer* renderer, bool colorkey){
    loaded = false;
    size.x=0;
    size.y=0;
    size.h=h;
    size.w=w;
    
    // Copy renderer reference
    randr = renderer;
    
    // Load picture
    SDL_Surface *image_surface = IMG_Load(file);
    if (image_surface != NULL){
        loaded = true;
    } else {
        printf("Failed at loading %s\n", file);
    }
    
    // Apply color keying.
    color_key_surface(colorkey, image_surface);
    
    // Create the texture and clear it
    Texture = SDL_CreateTextureFromSurface(renderer, image_surface);
    SDL_FreeSurface(image_surface);
    
}

void Sprite::draw(void){
    if(loaded){
        SDL_RenderCopy(randr, Texture, NULL, &size);
    } else {
        printf("Attempted to draw a sprite that is not yet loaded");
    }
}

void Sprite::destroy(void){
    SDL_DestroyTexture(Texture);
}


void Sprite::setXY(int x, int y){
    size.x=x;
    size.y=y;
}

int Sprite::getX(void){
    return size.x;
}

int Sprite::getY(void){
    return size.y;

}

SDL_Texture* Sprite::get_texture(void){

	if (loaded){
		return Texture;
	}
	return NULL;

}


#endif
