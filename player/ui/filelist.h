

class filelist{
	private:
		engine *ref             = NULL;         	// Reference to the engine
		config *cfg             = NULL;         	// Reference to the config
        theme *ui               = NULL;         	// Reference to the theme config
        font *fnt               = NULL;         	// Reference to the font
		char path[MAX_PATH];
		directory current_dir;
		bool has_dir;
		int cursor_index;
		int current_start;
		void load_icons(void);
		SDL_Rect main_rect;
		
		SDL_Surface *folder		= NULL;
		SDL_Surface *music		= NULL;
		SDL_Surface *playlist	= NULL;
		SDL_Surface *unknown	= NULL;
		
		SDL_Texture *framebuffer= NULL;
		

	public:
		filelist(const char* initial_directory, engine *engine, config *maincfg, theme *thm, font *newfont);
		void draw(void);
		void loop(void);
		void set_height(int h);
		void set_width(int w);
		void setX(int x);
		void setY(int y);


};




filelist::filelist(const char* initial_directory, engine *engine, config *maincfg, theme *thm, font *newfont) : current_dir(initial_directory){
	has_dir = current_dir.is_valid();
	if (!has_dir){
		printf("Unable to open starter directory in config.ini, going to root instead\n");
		if (current_dir.load_dir("/")){
			has_dir = true;
			printf("Fall back worked! Resuming\n");
		} else {
			printf("Unable to open / directory. Failing at directory listing...\n");
			exit(-255);
		}
	}
	ref = engine;
	cfg = maincfg;
	ui  = thm;
	fnt = newfont;
	
	cursor_index = 0;
	current_start= 0;

	main_rect.y=0;
	main_rect.x=0;
	
	load_icons();
}


void filelist::set_height(int height){
	main_rect.h = height;
}

void filelist::set_width(int width){
	main_rect.w = width;
}

void filelist::setY(int y){
	main_rect.y = y;
}

void filelist::setX(int x){
	main_rect.x = x;
}


void filelist::load_icons(void){

	char folder_path[MAX_PATH];
	char music_path[MAX_PATH];
	char playlist_path[MAX_PATH];
	char unknown_path[MAX_PATH];

	sprintf(folder_path, 	"./%sfolder.png", 	ui->icon_path);
	sprintf(music_path,		"./%smusic.png",	ui->icon_path);
	sprintf(playlist_path,	"./%splaylist.png",	ui->icon_path);
	sprintf(unknown_path,	"./%sunknown.png", 	ui->icon_path);
			    
	folder 	= IMG_Load(folder_path);
	music	= IMG_Load(music_path);
	playlist= IMG_Load(playlist_path);
	unknown = IMG_Load(unknown_path);
	
	color_key_surface(true, folder);
	color_key_surface(true, music);
	color_key_surface(true, playlist);
	color_key_surface(true, unknown);
	
	if (folder == NULL || music == NULL || playlist == NULL || unknown == NULL){
		printf("One or more icons failed to load. The UX might suffer because of this...\n");
	}
	

}


void filelist::draw(void){
	
	if (main_rect.h == 0 || main_rect.w == 0){
		printf("Attempted to draw a filelist component without specifying height and/or width\n");
		return;
	}
	SDL_Surface *temp = SDL_CreateRGBSurface(0,main_rect.w,main_rect.h,16,0,0,0,0);
	SDL_Rect icon_rect;
	
	fnt->setX( (ui->font_size*2) + (int)(ui->font_size/2));
	int i;
	
	icon_rect.x = ui->font_size + (int)(ui->font_size/4);
	icon_rect.y = 0;
	icon_rect.h=ui->font_size;
	icon_rect.w=ui->font_size;
	
	
	for (i=0;i<current_dir.length();i++){
		int posY = i*ui->font_size;
		
		if (posY+ui->font_size > main_rect.h){ break; }
		icon_rect.y = posY;
		
		if (current_dir.is_folder(i)){
			SDL_BlitSurface(folder, NULL , temp, &icon_rect);
		} else{
			char *ext = current_dir.get_file_extension(i);
			if (ext == NULL){
				SDL_BlitSurface(unknown, NULL , temp, &icon_rect);
			} else {
				bool mus = (strcmp(ext,"mp3")==0)+(strcmp(ext,"flac")==0)+
						   (strcmp(ext,"ogg")==0)+(strcmp(ext,"s3m")==0)+
						   (strcmp(ext,"xm")==0)+(strcmp(ext,"it")==0)+
						   (strcmp(ext,"mod")==0)+(strcmp(ext,"wav")==0);
				bool m3u = (strcmp(ext,"m3u")==0);
				if (m3u){
					SDL_BlitSurface(playlist, NULL , temp, &icon_rect);
				} else if (mus){
					SDL_BlitSurface(music, NULL , temp, &icon_rect);
				} else {
					SDL_BlitSurface(unknown, NULL , temp, &icon_rect);
				}
			}
		}
		
		fnt->setY(i*ui->font_size);
		fnt->draw_to_surface(current_dir.get_file(i), temp);	
	}
	//SDL_Rect draw;
	SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0, 0));
	framebuffer = SDL_CreateTextureFromSurface(ref->renderer, temp);
	SDL_RenderCopy(ref->renderer, framebuffer, NULL, &main_rect);
	SDL_FreeSurface(temp);
	
	
}

void filelist::loop(void){
	//STUB
}