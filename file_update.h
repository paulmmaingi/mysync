#include "file_difference.h"
#include <utime.h>
#include <sys/time.h>


// --------------------------------------------------------------------
// FUNCTION DECLARATIONS

// function to actually create empty directories
extern void _mkdir(const char *dir, option_list* option_list);

// create the empty directories that need to be created
extern void create_empty_dirs(mod_tup_list* list, option_list* option_list);

// uses the modification tuple list to carry out the modifications
extern void process_mod_tup_list(mod_tup_list* list, hash_table *table, option_list* option_list);