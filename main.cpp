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
#include "io/display.h"

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
display lcd(maincfg.get_string_by_key("backlight"));


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
        //madamp.clear_screen();        
        joy.poll();
        
        main_loop = (false == (joy.start && joy.select));
        

        bg.draw();
        playlist.draw();
      
        playlist.loop();
        madamp.loop();
    }

	playlist.destroy();
    bg.destroy();
    madamp.destroy();

}
