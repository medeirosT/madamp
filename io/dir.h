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

/**
*	Class initializer
*	@param {const char*} newdir -- The directory we're starting at.
*/
directory::directory(const char* newdir){
	strcpy(path, newdir);
	valid = load_dir(path);
}


/**
*	Goes up a directory and calls @load_dir
*	@return {bool} the success of the function
*/
bool directory::go_up(void){
	// Create a copy of the path we're in.
	char temp_path[MAX_PATH];
	strcpy(temp_path, path);
	
	// Prepare the string manipulation
	int start = -1;
	int end = -1; 
	long unsigned int i;
	
	// what we're doing here is looking for the second to last "/" character
	// And adding a terminator "/0" after it to close up the array, eliminating
	// a directory
	for(i=0;i<strlen(temp_path);i++){
		if (temp_path[i] == '/'){
			start = end;
			end = i;	
		}
	}
	// Terminate it
	temp_path[start+1]='\0';
	
	// Copy back to the path variable
	strcpy(path, temp_path);
	
	// Load up the new directory
	return load_dir(path);
	
}

bool directory::open_folder(int index){
	// Make sure what we're opening is actually a folder
	if (folder[index] == false || index > file_count){
		return false;
	}
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
