

class filelist{
	private:
		char path[MAX_PATH];
		directory current_dir;
		bool has_dir;
		int cursor_index;
		
		
	public:
		filelist(const char* initial_directory);
		void draw(void);
		void loop(void);


};




filelist::filelist(const char* initial_directory) : current_dir(initial_directory){
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
}



void filelist::draw(void){
	// STUB
}

void filelist::loop(void){
	//STUB
}