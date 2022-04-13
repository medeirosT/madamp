#define MAX_PATH 4096	// Maximum # of characters allowed in a path variable
#include <dirent.h>


class directory{
    private:
    	char path[MAX_PATH];
        bool valid;   
        char files[256][128];
        bool folder[256];
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
	char temp_path[MAX_PATH];
	strcpy(temp_path, path);
	int start = -1;
	int end = -1; 
	long unsigned int i;
	for(i=0;i<strlen(temp_path);i++){
		if (temp_path[i] == '/'){
			start = end;
			end = i;	
		}
	}
	temp_path[start+1]='\0';
	strcpy(path, temp_path);
	return load_dir(path);
	
}

bool directory::open_folder(int index){
	// Index = 0 is always ".." so, go up
	if (index == 0){
		return go_up();
	
	// If not, and it is an actual folder, add it!
	} else {
		char temp_path[MAX_PATH-1];
		strcpy(temp_path, path);
		sprintf(path, "%s%s/", temp_path, get_file(index));
		return load_dir(path);
	}
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
            if( strcmp(dir->d_name,".") != 0 ){      
                strcpy(files[file_count], dir->d_name );
                folder[file_count] = (dir->d_type == DT_DIR);
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
