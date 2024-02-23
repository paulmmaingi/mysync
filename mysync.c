//  CITS2002 Project 2 2023; Written by PAUL MAINGI

// Project description: https://teaching.csse.uwa.edu.au/units/CITS2002/projects/project2.php

// order of files: directorystruct.h, directorystruct.c, hashtable.h, hashtable.c, glob2regex.h, glob2regex.c, read_directories.h, read_directories.c, file_difference.h, file_difference.c, file_update.h, file_update.c, mysync.h, mysync.c. + a Makefile 




#include "mysync.h"

#define	OPTLIST		"ai:no:prv"

int debug1 = 0;


// --------------------------------------------------------------------
// FUNCTION DEFINITIONS

void usage(char* progname) {
    fprintf(stderr, "Usage: %s [options] directory1  directory2  [directory3  ...]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -a           Synchronise all files, including hidden and configuration files.\n");
    fprintf(stderr, "  -i <pattern>     Ignore files matching the specified pattern.\n");
    fprintf(stderr, "  -n           Identify files to be synchronised, but do not actually synchronise them. Activates -v\n");
    fprintf(stderr, "  -o <pattern>     Only synchronise files matching the specified pattern.\n");
    fprintf(stderr, "  -p           Preserve the modification time and file permissions of copied files.\n");
    fprintf(stderr, "  -r           Recursively synchronise files in subdirectories.\n");
    fprintf(stderr, "  -v           Print verbose output to stdout.\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char* argv[]) {
    char* progname = argv[0];
   
    // Check that there are at least two directories to compare
    if (argc < 3) {
        fprintf(stderr, "Error: at least two directories must be specified\n");
        usage(progname);
        return 1;
    }

    int opt;

    // option list
    option_list* list = new_option_list();

    // Parse the command-line options
    while ((opt = getopt(argc, argv, OPTLIST)) != -1) {
        switch (opt) {
            case 'a':
                add_option(list, 'a', "");
                break;
            case 'i':
                add_option(list, 'i', optarg);
                break;
            case 'n':
                add_option(list, 'n', "");
                break;
            case 'o':
                add_option(list, 'o', optarg);
                break;
            case 'p':
                add_option(list, 'p', "");
                break;
            case 'r':
                add_option(list, 'r', "");
                break;
            case 'v':
                add_option(list, 'v', "");
                break;
            default:    //  invalid command-line option
                usage(progname);
                return 1;
        }
    }

    // combine options
    list = combine_options(list);
    
    // print given options
    for(int i = 0; i < list->num_options; i++){
        printf("Option %d (%d): %c", i+1, list->options[i]->num_patterns , list->options[i]->flag);
        if(list->options[i]->num_patterns > 0){
            printf(" Patterns:");
            for(int j = 0; j < list->options[i]->num_patterns; j++){
                printf(" '%s'", list->options[i]->patterns[j]);
            }
        }
        printf("\n");
    }

    // remove options from argv and reduce argc
    argc -= optind;
    argv += optind;

    // error check again for directories
    if (argc < 1) {
        usage(progname);
        return 1;
    }

    // print given directories
    for (int i = 0; i < argc; i++) {
        (debug1 == 1 || debug1 == 0) ? printf("Directory %d: %s\n", i + 1, argv[i]) : 0;
    }
    printf("\n");

    // --------------------------------------------------------------------

    // verbose output with -v or -n which sets -v
    if(find_option(list, 'v') || find_option(list, 'n')){
        debug1 = 1;
    }

    // file system before syncing
    debug1 == 1? printf("\nFile system before sync:\n\n") : 0;

    // Create a new hash table
    hash_table* table = new_hash_table(HASHTABLE_SIZE);

    // Create a new modification tuple list
    mod_tup_list* mod_tup_list = new_mod_tup_list();

    // Scan the directories and add them to the hash table
    for (int i = 0; i < argc; i++) {
        scan_directory(argv[i], table, list);
    }

    // print the hash table if -v is in the option list
    debug1 == 1 ? print_hash_table(table) : 0;

    // update the modification tuple list
    mod_tup_list = update_entire_mod_tup_list(table, mod_tup_list, argc, argv, list);

    // print the modification tuple list if -n is in the option list then exit
    if(find_option(list, 'n')){
        print_mod_tup_list(mod_tup_list);

        // to print what would be created
        create_empty_dirs(mod_tup_list, list);

        free_hash_table(table);
        free_option_list(list);
        free_mod_tup_list(mod_tup_list);
        printf("No files were synchronised\n");
        return 0;
    }

    // print the modification tuple list if -v is in the option list
    debug1 == 1 ? print_mod_tup_list(mod_tup_list) : 0;

    // create the empty directories
    create_empty_dirs(mod_tup_list, list);

    // update the files
    process_mod_tup_list(mod_tup_list, table, list);    

    // get file system after update for verbose
    if(debug1 == 1 && (!find_option(list, 'n'))){
        printf("\nFile system after sync:\n\n");
        hash_table* table2 = new_hash_table(HASHTABLE_SIZE);
        for (int i = 0; i < argc; i++) {
            scan_directory(argv[i], table2, list);
        }
        print_hash_table(table2);
        free_hash_table(table2);
    }
    if(mod_tup_list->num_mod_tups > 0){
        printf("Files synchronised successfully\n"); 
    }
    else if(mod_tup_list->num_mod_tups == 0) { 
        printf("No files needed to be synchronised\n");
    }
     
    // Free memory
    free_hash_table(table);
    free_option_list(list);
    free_mod_tup_list(mod_tup_list);

    return 0;
}