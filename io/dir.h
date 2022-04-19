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
        char * get_file_extension(int index);
		bool load_dir(const char* newdir);
		bool go_up(void);
		bool open_folder(int index);
		bool is_folder(int index);
		bool is_valid(void);
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


/**
*	Opens the folder by index (Safer than going by character. This way we have
*	direct access to all file info easily. no need for aditional checks)
*	@param {int} index -- index of the folder we're attempting to open
*	@return {bool} whether or not we've succeeded in opening said folder
*/
bool directory::open_folder(int index){
	// Make sure what we're opening is actually a folder
	if (folder[index] == false || index > file_count) return false;
	
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

/**
*	Change directories basically.
*	@param {const char*} newdir - new directory we're changing to.
*	@return {bool} success of the operation. 
*/
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

/**
*	Returns the amount of files in the current directory
*	@param {int} number of files in this folder.
*/
int directory::length(void){ return file_count; }


/**
*	Returns whether or not an index is a folder or not.
*	@param {int} index - index of the file/folder we're checking
*	@return {bool} whether or not an index file/folder is a folder.
*/
bool directory::is_folder(int index){ return folder[index]; }

/**
*	returns whether or not we're successful in loading a directory's content
*	@return {bool} returns valid status (true = success, false = failure)
*/
bool directory::is_valid(void){ return valid;}

/**
*	Gets the name of the file by index.
*	@param {int} index -- index of the file/folder we're getting
*	@return {char *} -- name of the file in question.*
*/
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

char * directory::get_file_extension(int index){
	char filename[260];
	char extension[20];
	int i;
	int last_dot=-1;
	int extension_index=0;
	
	if (index == 0){ return NULL; }
	
	strcpy(filename, get_file(index));

	for(i=0;i<(int)strlen(filename);i++){
		if(filename[i]=='.'){
			last_dot=i;
		}
	}
	if (last_dot == -1){ return NULL; }
	for(i=last_dot+1;i<(int)strlen(filename);i++){
		// characters below 97 are upper case. Just add 22 to make them lower
		extension[extension_index]=filename[i] + (filename[i]<97 && filename[i] > 65  ? 32 : 0);
		extension_index++;
	}
	extension[extension_index]='\0';
	char *output = (char *)malloc(strlen(extension)+1);
	sprintf(output, "%s", extension);
	
	return output;
}
