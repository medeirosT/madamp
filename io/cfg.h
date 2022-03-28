#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cstdlib>

#define MAX_KEY_SIZE 32
#define MAX_VAL_SIZE 64
#define MAX_LEN MAX_KEY_SIZE+1+MAX_VAL_SIZE

class config{
	private:
	    char filename[256];               // filename 
	    char data_keys[64][MAX_KEY_SIZE];   // keys by index
	    char data_vals[64][MAX_VAL_SIZE];   // values by index
	    int data_count;                     // number of key/value combos we have
	    bool loaded;                        // whether or not we've loaded the config file
	    void parse_line(char* line);        // internal function to parse config file.
	    
	    char * get_key_from_line(const char *line);
	    char * get_val_from_line(const char *line);
	    
	public:
	    config (const char* file);                                      // constructor
	    char * get_string_by_key( const char *key );                    // get value by key, return char array
        int get_int_by_key( const char *key, int safedefault = -1);     // get value by key, return int
        bool set_int (const char *key, int value);		// Update the value on the ini file
        bool set_string (const char *key, const char *value);	// Update the value on the ini file
};

/**
*   Constructor for the config object
*   Takes a filename and attempts to open it and beings to parse it.
*   @param file {const char*} - Filename to open. Uses INI format sans [] headers.
*/
config::config(const char* file){
	
	strcpy(filename, file);

    // Declare a FILE handler and open for "R"eading
    FILE* fp;
    fp = fopen(file, "r");
    
    // Check if we loaded..
    loaded = (fp != NULL);
    if (!loaded){ 
        printf("Error loading config. Giving up...\n"); 
        return;
    } 
    
    // Ok, create a buffer
    char buffer[MAX_LEN];
    
    // initialize the config count
    data_count=0;
    
    // Read line by line
    while(fgets(buffer, MAX_LEN, fp)){
    
        // parse each line
        parse_line(buffer);    
    }
    
    // close the file
    fclose(fp);
    
}


char * config::get_key_from_line(const char *line){
	int i = 0;
	char key[MAX_KEY_SIZE];
	
	while(line[i] && line[i]!= '='){
		key[i]=line[i];
	
		i++;
	}
	key[i]='\0';
	char *output = (char *)malloc(strlen(key)+1);
	sprintf(output,"%s", key);
	return output;
	
}

char * config::get_val_from_line(const char *line){
	int i=0;
	int val_index=0;
	char val[MAX_VAL_SIZE];
	bool start_copying = false;
	while(line[i]){
		if (start_copying){
			val[val_index] = line[i];
			val_index++;
		}
	
		if (line[i]=='='){ start_copying = true; };
		i++;
		
	}
	val[val_index]='\0';
	char *output = (char *)malloc(strlen(val)+1);
	sprintf(output,"%s", val);
	return output;
}


/**
*   This function parses each line, separates key from value
*   and assigns values to data_keys and data_vals
*   @param line {char*} -- line we're parsing. Usually called from constructor.
*/
void config::parse_line(char* line){

    
    // if the line starts with a ; or a # assume comment
    bool ignore_line = line[0]=='#' || line[0]==';' || line[0]==' ' || line[0] == '\0' || line[0] == '[';   
        
    if (!ignore_line){
    
    	strcpy(data_keys[data_count],get_key_from_line(line));
    	strcpy(data_vals[data_count],get_val_from_line(line));
    	data_count++;
    
    
    }
    

}

/**
*   I hate this function, but because this is C++ we have to do things
*   ... differently. I have to create a pointer, copy my value to it and
*   return *that*. 
*   ... It works. Not touching this again >:C
*
*   @param {key} const char * - key we're looking for
*   @return char * - value of the key we're looking for. NULL if we can't find it
*
*/
char * config::get_string_by_key( const char *key) {

    // Loop through al keys
    for (int i=0;i<data_count;i++){
    
        // check if both arrays match
        if (strcmp(key, data_keys[i]) == 0){
        
            // create the forementioned pointer with enough memory allocated to it
            char *output = (char *)malloc(strlen(data_vals[i]+1));
            
            // Copy actual value to pointer
            // sans the terminating character. That was causing issues
            sprintf(output, "%.*s", (int)(strlen(data_vals[i])-1), data_vals[i]);
            
            
            // return it
            return output;
        }
    }
    // or, ya know, return null
    // Not like I like you or anything
    return NULL;
}


bool config::set_string (const char *key, const char *value){
	
    // Declare a FILE handler and open for "R"eading
    FILE* fp;
    FILE* tmp;

    char buffer[MAX_LEN];
    char out[MAX_LEN];
    
    fp = fopen(filename, "r");
    tmp= fopen("tmp.cfg", "w");
    // Check if we loaded..
    if (fp == NULL || tmp == NULL){ 
        printf("Error either opening temp file to write, or opening config...\n"); 
        return false;
    }

    // Read line by line
    while(fgets(buffer, MAX_LEN, fp)){
    
        if (strcmp(key, get_key_from_line(buffer)) == 0){
        	
        	sprintf(out, "%s=%s\n",key,value);
        	fputs(out, tmp);
        } else {
        	fputs(buffer,tmp);
        }
        
		if( feof(fp) ) { 
			break ;
		} 
    }
    
    // close the file
    fclose(fp);
    fclose(tmp);
    
    remove(filename);
    rename("tmp.cfg",filename);
    
    return true;


}

bool config::set_int (const char *key, int value){
	
	char theint[MAX_VAL_SIZE];
	
	sprintf(theint, "%d", value);
	return set_string(key, theint);
}




/**
*   same function as above, only it returns in integer instead
*   @param {key} const char * - key we're looking for
*   @param {default} int      - What to return in case we fail;
*   @return int - value of the key we're looking for. NULL if we can't find it
*/
int config::get_int_by_key( const char *key, int safedefault){

    // use the above code to find the raw value
    char *val = get_string_by_key(key);
    
    // check for null, return -1 if null
    if (val == NULL){
        printf("Attempted to load %s from %s and couldn't find it, returning default of %d\n", key, filename, safedefault);
        return safedefault;
    } 
    
    // if all checks out, return the integer interpreted.
    return atoi(get_string_by_key(key));

	delete val;
}

