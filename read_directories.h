#include "glob2regex.h"

#include <regex.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dirent.h>
#include  <fcntl.h>
#include  <getopt.h>


// --------------------------------------------------------------------

// FUNCTION DECLARATIONS

// returns true if file is hidden
extern int is_hidden_file(const char *filename);

// function to handle directory scanning
extern void scan_directory(char *dirname, hash_table* table, option_list* list);