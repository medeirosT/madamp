#define MAX_PATH 4096	// Maximum # of characters allowed in a path variable
#include <dirent.h>


class directory{
    private:
    	char path[MAX_PATH];
        bool valid;   
        char files[256][128];
        int file_count;
    public:
        directory(const char* newdir);
        int length(void);
        char * get_file(int index);
		bool load_dir(const char* newdir);
		bool go_up(void);
		bool open_folder(int index);
};


directory::directory(const char* newdir){
	strcpy(path, newdir);
	valid = load_dir(path);
}


bool directory::go_up(void){
	

}

bool directory::open_folder(int index){


}

bool directory::load_dir(const char* newdir){
	
	valid = false;
    file_count=0;
    DIR *d;
    struct dirent *dir;
    d = opendir(newdir);
    if(d){
    	valid = true;
        while((dir = readdir(d)) != NULL){
            if( strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,".") != 0 ){      
                strcpy(files[file_count], dir->d_name );
                file_count++;
            }
        }
        closedir(d);
    }
	return valid;
}


int directory::length(void){ return file_count; }

char * directory::get_file(int index){
    if (index <= file_count && valid){
        char *output = (char *)malloc(strlen(files[index])+1);
        // Copy actual value to pointer
        // sans the terminating character. That was causing issues
        sprintf(output, "%s", files[index]);
        // return it
        return output;
    
    }
    return NULL;
}
