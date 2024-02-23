#include "read_directories.h"


// ----------------------------------------------
// DATA STRUCTUREs

//modification tuple (filepath, origin directory, destination directory)
typedef struct mod_tup{
    char* filepath;
    char* origin_dir;
    char* destination_dir;
} mod_tup;

// modification tuple list (array of modification tuples, number of modification tuples)
typedef struct mod_tup_list{
    mod_tup** mod_tups;
    int num_mod_tups;
} mod_tup_list;


// ----------------------------------------------
// FUNCTION DECLARATIONS

// compares the modification time of two files and returns the later file
extern file* compare_mtime(file* file1, file* file2);

// creates a new modification tuple, returns pointer to it
extern mod_tup* new_mod_tup(char* filepath, char* origin_dir, char* destination_dir);

// create a new modification tuple list, returns pointer to it
extern mod_tup_list* new_mod_tup_list(void);

// add a modification tuple to the modification tuple list
extern void add_mod_tup(mod_tup_list* list, mod_tup* mod_tup);

// free the memory used by the modification tuple list
extern void free_mod_tup_list(mod_tup_list* list);

// print the modification tuple list
extern void print_mod_tup_list(mod_tup_list* list);

// goes through all the files in dir1 and dir2 and creates a mini modification tuple list for those directories, if -r is in the option list, it will add the modtuplelists for the subdirectories and create the subdirectories in the destination directory
extern mod_tup_list* create_mini_mod_tup_list(directory* dir1, directory* dir2, option_list* option_list, hash_table* table);

// concatenates two modification tuple lists
extern mod_tup_list* union_mod_tup_lists(mod_tup_list* list1, mod_tup_list* list2);

// creates the full modification tuple list for all directories
extern mod_tup_list* update_entire_mod_tup_list(hash_table* table, mod_tup_list* session_mod_tup_list, int ndirs, char** argv, option_list* option_list);