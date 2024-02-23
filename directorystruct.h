#include <stdio.h>  
#include <stdlib.h> 
#include  <stdbool.h>
#define _POSIX_C_SOURCE  200809L    
#include <string.h>      // for string functions
#include <sys/param.h>    // for MAXPATHLEN
#include <time.h>        // for ctime()


//  A HELPFUL PREPROCESSOR MACRO TO CHECK IF ALLOCATIONS WERE SUCCESSFUL
#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

// declaration of strdup
extern char *strdup(const char *s);


// ----------------------------------------------------------------
//  DATA STRUCTURES

// Define a struct for a file (name, mtime, perms)
typedef struct file {
    char *name;
    time_t mtime;
    char* perms;
} file;

// Define a struct for a directory (name, children, num_children, files, num_files)
typedef struct directory {
    char *name;
    struct directory** children;
    int num_children;
    struct file** files;
    int num_files;
} directory;

// Define a struct for an option and its patterns (flag, pattern)
typedef struct option_pattern {
    char flag;
    char** patterns;
    int num_patterns;
} option_pattern;

// Define a struct for a list of options and their patterns (options, num_options)
typedef struct option_list {
    option_pattern** options;
    int num_options;
} option_list;


// ----------------------------------------------------------------
//  FUNCTION DECLARATIONS

// creates new directory struct, returns pointer to it
extern directory* new_directory(char* name);

// prints directory struct given pointer to it
extern void print_directory(directory* dir);

// adds file to directory
extern void add_file(directory* dir, char* name);

// checks if file is in directory
extern bool file_in_directory(directory* dir, char* name);

// gets file from directory and returns pointer to it
extern file* get_file(directory* dir, char* name);

// adds subdirectory to directory -> updates parent details
extern void sub_directory(directory* dir, directory* child);

// frees directory memory
extern void free_directory(directory* dir);

// ----------------------------------------------------------------

// creates an option list struct and returns pointer to it
extern option_list* new_option_list();

// add an option to the option list
extern void add_option(option_list* list, char flag, char* pattern);

// returns an option_pattern struct for a given flag
extern option_pattern* get_option(option_list* list, char flag);

// puts together options that have the same flag
extern option_list* combine_options(option_list* list);

// finds if an option is in the option list
extern bool find_option(option_list* list, char flag);

// frees the option list
extern void free_option_list(option_list* list);