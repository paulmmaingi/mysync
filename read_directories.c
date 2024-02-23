#include "read_directories.h"

// debug flag
int debug = 0;


// --------------------------------------------------------------------
// FUNCTION DEFINITIONS

int is_hidden_file(const char *filename) {
    return (filename[0] == '.');
}

void scan_directory(char *dirname, hash_table* table, option_list* list) {
    DIR             *dirp;
    struct dirent   *dp;

    //  ENSURE THAT WE CAN OPEN (read-only) THE REQUIRED DIRECTORY
    dirp = opendir(dirname);
    if(dirp == NULL) {
        perror( dirname );
        exit(EXIT_FAILURE);
    }

    // Create a new directory struct for the current directory
    directory* current_dir = new_directory(dirname);

    // Add the current directory to the hash table
    add_directory_to_hash_table(table, current_dir);

    //  READ FROM THE REQUIRED DIRECTORY AND ADD TO HASH TABLE
    char pathname[MAXPATHLEN];
    while((dp = readdir(dirp)) != NULL) {  
        struct stat     stat_info;

        sprintf(pathname, "%s/%s", dirname, dp->d_name);
    
        //  DETERMINE ATTRIBUTES OF THIS DIRECTORY ENTRY
        if(stat(pathname, &stat_info) != 0) {
            perror(pathname);
            exit(EXIT_FAILURE);
        }

        // ADD THE FILES TO THE CURRENT DIRECTORY AND UPDATE THE MTIME
        if (S_ISREG(stat_info.st_mode)) {

            // Ignore hidden files unless the -a flag is used
            if (is_hidden_file(dp->d_name) && !find_option(list, 'a')) {
                continue;
            }

            // check if the file matches any of the patterns specified by the -i option and ignore it if it does
            if (find_option(list, 'i')) {
                bool match = false;
                for (int i = 0; i < get_option(list, 'i')->num_patterns; i++) {
                    regex_t regex;
                    int reti;
                    char* regex_string = glob2regex(get_option(list, 'i')->patterns[i]); 
                    if (regex_string == NULL) {
                        fprintf(stderr, "Error converting glob to regex\n");
                        exit(EXIT_FAILURE);
                    }
                    reti = regcomp(&regex, regex_string, 0);
                    if (reti) {
                        fprintf(stderr, "Could not compile regex\n");
                        exit(EXIT_FAILURE);
                    }
                    reti = regexec(&regex, dp->d_name, 0, NULL, 0);
                    if (!reti) {
                        match = true;
                        break;
                    }
                    else if (reti == REG_NOMATCH) {
                    }
                    else {
                        regerror(reti, &regex, regex_string, sizeof(regex_string));
                        fprintf(stderr, "Regex match failed: %s\n", regex_string);
                        exit(EXIT_FAILURE);
                    }
                    regfree(&regex);
                }
                if (match) {
                    continue;
                }
            }
            
            // check if the file matches any of the patterns specified by the -o option and ignore it if it doesn't
            if (find_option(list, 'o')) {
                bool match = false;
                for (int i = 0; i < get_option(list, 'o')->num_patterns; i++) {
                    regex_t regex;
                    int reti;
                    char* regex_string = glob2regex(get_option(list, 'o')->patterns[i]); 
                    if (regex_string == NULL) {
                        fprintf(stderr, "Error converting glob to regex\n");
                        exit(EXIT_FAILURE);
                    }
                    reti = regcomp(&regex, regex_string, 0);
                    if (reti) {
                        fprintf(stderr, "Could not compile regex\n");
                        exit(EXIT_FAILURE);
                    }
                    reti = regexec(&regex, dp->d_name, 0, NULL, 0);
                    if (!reti) {
                        match = true;
                        break;
                    }
                    else if (reti == REG_NOMATCH) {
                    }
                    else {
                        regerror(reti, &regex, regex_string, sizeof(regex_string));
                        fprintf(stderr, "Regex match failed: %s\n", regex_string);
                        exit(EXIT_FAILURE);
                    }
                    regfree(&regex);
                }
                if (!match) {
                    continue;
                }
            }

            // Add the file to the current directory
            add_file(current_dir, dp->d_name);
            // Update the mtime of the file
            current_dir->files[current_dir->num_files - 1]->mtime = stat_info.st_mtime;
            // Update the permissions of the file
            current_dir->files[current_dir->num_files - 1]->perms = malloc(10 * sizeof(char));
            CHECK_ALLOC(current_dir->files[current_dir->num_files - 1]->perms);
            sprintf(current_dir->files[current_dir->num_files - 1]->perms, "%c%c%c%c%c%c%c%c%c",
                    (stat_info.st_mode & S_IRUSR) ? 'r' : '-',
                    (stat_info.st_mode & S_IWUSR) ? 'w' : '-',
                    (stat_info.st_mode & S_IXUSR) ? 'x' : '-',
                    (stat_info.st_mode & S_IRGRP) ? 'r' : '-',
                    (stat_info.st_mode & S_IWGRP) ? 'w' : '-',
                    (stat_info.st_mode & S_IXGRP) ? 'x' : '-',
                    (stat_info.st_mode & S_IROTH) ? 'r' : '-',
                    (stat_info.st_mode & S_IWOTH) ? 'w' : '-',
                    (stat_info.st_mode & S_IXOTH) ? 'x' : '-');
        }


        // ADD THE SUBDIRECTORIES TO THE CURRENT DIRECTORY
        if (S_ISDIR(stat_info.st_mode)) {
            // Ignore the "." and ".." directories
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                continue;
            }
            
            // add the subdirectory to the current directory
            directory* sub_dir = new_directory(dp->d_name);
            sub_directory(current_dir, sub_dir);

            // Add all the subdirectories to the hash table and then scan them if the -r option is used
            if (find_option(list, 'r')) {
                char* sub_dir_path = malloc(sizeof(char) * (strlen(dirname) + strlen(dp->d_name) + 2));
                CHECK_ALLOC(sub_dir_path);
                strcpy(sub_dir_path, dirname);
                strcat(sub_dir_path, "/");
                strcat(sub_dir_path, dp->d_name);
                scan_directory(sub_dir_path, table, list);
                free(sub_dir_path);
            }
        } 
    }
    debug == 1 ? printf("name: %s\n", current_dir->name) : 0;
    debug == 1 ? printf("num_children: %d\n", current_dir->num_children) : 0;
    debug == 1 ? printf("num_files: %d\n\n", current_dir->num_files) : 0;

    //  CLOSE THE DIRECTORY
    closedir(dirp);
}