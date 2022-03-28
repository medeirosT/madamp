#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include "engine.h"
#include "gfx/sprite.h"
#include "gfx/background.h"
#include "gfx/font.h"
#include "io/joy.h"
#include "io/cfg.h"
#include "io/dir.h"

// player specific:
#include "player/theme.h"
#include "player/length.h"
#include "player/list.h"

using namespace std;

bool main_loop = true;
config maincfg("config.ini");
engine madamp(maincfg.get_int_by_key("screenwidth",480),maincfg.get_int_by_key("screenheight",320),maincfg.get_int_by_key("framerate",60));
joystick joy(0);
Mix_Music *loop;


int main() {
    
    
    
    madamp.init_gfx();
    madamp.init_sfx();
    madamp.disable_debug_mode();
    theme maintheme(maincfg.get_string_by_key("theme"));
    
    
    background bg(maintheme.file_bg,madamp);
    font fnt(maintheme.file_font,maintheme.font_size,madamp);
    fnt.setRGBSDL(&maintheme.font_color);
    directory musicdir(maincfg.get_string_by_key("musicdir"));


    
    
    
    list playlist(&madamp, &fnt,&maincfg,&maintheme, &joy, &musicdir, loop);
    
    while ( main_loop )
    {
        madamp.clear_screen();        
        joy.poll();
        
        main_loop = (false == (joy.start && joy.select));
        

        bg.draw();
        playlist.draw();
        
        /**
        
        fnt.setX(0);
        fnt.setY(cursor_index*(maintheme.font_size+2));
        fnt.draw(maincfg.get_string_by_key("cursor"));
        fnt.setX(maintheme.font_size*strlen(maincfg.get_string_by_key("cursor"))+2);
        int i=0;
        for(i=0;i<musicdir.length();i++){
            fnt.setY(i*(maintheme.font_size+2));
            fnt.draw(musicdir.get_file(i));
        }
        
        if (joy.up){
            if (cursor_index > 0){
                cursor_index--;
            }
        } else if (joy.down){
            cursor_index = (cursor_index+1)%((int)(maincfg.get_int_by_key("screenheight",320)/maintheme.font_size)-1);
        } else if (joy.a){
            if (musicdir.get_file(cursor_index)!=NULL){
                char *fullpath = (char *)malloc(strlen(musicdir.get_file(cursor_index))+strlen(maincfg.get_string_by_key("musicdir")));
                sprintf(fullpath,"%s%s", maincfg.get_string_by_key("musicdir"),musicdir.get_file(cursor_index));
                loop = Mix_LoadMUS(fullpath);
                if (!loop){
                    printf("Could not load music! SDL_Mixer : %s\n", Mix_GetError() );
                } else {
                    if(Mix_PlayMusic(loop, -1)==-1){
                        printf("Error playing : %s\n", Mix_GetError());
                    } else {
                        printf("Playing\n");
                    }
                    
                }
            }
        }
        */
        
        //fnt.draw("Hello World");
        
        //fnt.setRGB(main.font_color.r,main.font_color.g,main.font_color.b);
        //r=(r+1)%254;g=(g+1)%254;b=(b+1)%254;
        
        playlist.loop();
        madamp.loop();
    }

	playlist.destroy();
    bg.destroy();
    madamp.destroy();

}
