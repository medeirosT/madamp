#include <dirent.h>


class directory{
    private:
        const char* dirpath;
        bool valid;   
        char files[256][128];
        int file_count;
    public:
        directory(const char* newdir);
        int length(void);
        char * get_file(int index);
};


directory::directory(const char* newdir){
    dirpath = newdir;
    valid = false;
    file_count=0;
    DIR *d;
    struct dirent *dir;
    d = opendir(dirpath);
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


}

int directory::length(void){ return file_count; }

char * directory::get_file(int index){
    if (index <= file_count){
        char *output = (char *)malloc(strlen(files[index])+1);
        // Copy actual value to pointer
        // sans the terminating character. That was causing issues
        sprintf(output, "%s", files[index]);
        // return it
        return output;
    
    }
    return NULL;
}
