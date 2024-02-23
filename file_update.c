#include "file_update.h"

// debug flag
int debug2 = 0;


// --------------------------------------------------------------------
// FUNCTION DEFINITIONS

void _mkdir(const char *dir, option_list* option_list) {
    char path[MAXPATHLEN];
    size_t len;

    // Copy the input directory path to a temporary buffer
    snprintf(path, sizeof(path), "%s", dir);

    // Remove any trailing slash from the path
    len = strlen(path);
    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }

    // Create each directory in the path one by one
    char *path_ptr = path + 1;
    while (*path_ptr) {
        if (*path_ptr == '/') {
            *path_ptr = '\0';
            if (access(path, F_OK) != 0) {
                if(find_option(option_list, 'v') && !find_option(option_list, 'n')){
                    printf("\nCreating directory %s\n\n", path);
                }
                if(find_option(option_list, 'n')){
                    printf("\nWould create directory %s\n\n", path);
                }
                else{
                    if(mkdir(path, 0777) == -1){
                        fprintf(stderr, "Error creating directory %s\n", path);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else{
                if(find_option(option_list, 'v') || find_option(option_list, 'n')){
                    printf("\n%s already exists, no need to create it\n\n", path);
                }
            }
            *path_ptr = '/';
        }
        path_ptr++;
    }

    // Create the final directory in the path
    if (access(path, F_OK) != 0) {
        if(find_option(option_list, 'v') && !find_option(option_list, 'n')){
            printf("\nCreating directory %s\n\n", path);
        }
        if(find_option(option_list, 'n')){
            printf("\nWould create directory %s\n\n", path);
        }
        else{
            if (mkdir(path, 0777) == -1) {
                fprintf(stderr, "Error creating directory %s\n", path);
                exit(EXIT_FAILURE);
            }
        }
    }
    else{
        if(find_option(option_list, 'v') || find_option(option_list, 'n')){
            printf("\n%s already exists, no need to create it\n\n", path);
        }
    }
}

void create_empty_dirs(mod_tup_list* list, option_list* option_list){
    // only if -r option is specified do we need to create empty directories
    if(!find_option(option_list, 'r')){
        return;
    }
    for (int i = 0; i < list->num_mod_tups; i++){
        // get destination directory
        char* destination_dir = list->mod_tups[i]->destination_dir;

        // create the directory
        _mkdir(destination_dir, option_list);
    }
}

void process_mod_tup_list(mod_tup_list* list, hash_table *table, option_list* option_list){
    // do nothing if the modification tuple list is empty
    if(list->num_mod_tups == 0){
        return;
    }
    for(int i = 0; i < list->num_mod_tups; i++){
        // get filename, destination directory and origin directory
        char* filename = list->mod_tups[i]->filepath;
        char* destination_dir = list->mod_tups[i]->destination_dir;
        char* origin_dir = list->mod_tups[i]->origin_dir;
        // printf("filename: %s, destination_dir: %s, origin_dir: %s\n", filename, destination_dir, origin_dir);

        // open origin file for reading
        char* full_origin = malloc(sizeof(char) * (strlen(origin_dir) + strlen(filename) + 2));
        CHECK_ALLOC(full_origin);
        strcpy(full_origin, origin_dir);
        strcat(full_origin, "/");
        strcat(full_origin, filename);
        // printf("full_origin: %s\n", full_origin);
        int fd1 = open(full_origin, O_RDONLY);
        if(fd1 == -1){
            fprintf(stderr, "Error opening file %s\t\t (read)\n", full_origin);
            free(full_origin);
            exit(EXIT_FAILURE);
        }

        // open destination file for writing
        char* full_destination = malloc(sizeof(char) * (strlen(destination_dir) + strlen(filename) + 2));
        CHECK_ALLOC(full_destination);
        strcpy(full_destination, destination_dir);
        strcat(full_destination, "/");
        strcat(full_destination, filename);
        // printf("full_destination: %s\n", full_destination);
        int fd2 = open(full_destination, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if(fd2 == -1){
            fprintf(stderr, "Error opening file %s\t\t (write)\n", full_destination);
            free(full_destination);
            free(full_origin);
            close(fd1);
            exit(EXIT_FAILURE);
        }

        // read from origin file and write to destination file
        char* buffer = malloc(sizeof(char) * 1024);
        CHECK_ALLOC(buffer);
        int bytes_read = 0;
        while((bytes_read = read(fd1, buffer, sizeof buffer)) > 0){
            if(write(fd2, buffer, bytes_read) != bytes_read){
                fprintf(stderr, "Error writing to file %s\n", full_destination);
                free(full_destination);
                free(full_origin);
                free(buffer);
                close(fd1);
                close(fd2);
                exit(EXIT_FAILURE);
            }
        }
        if(bytes_read == -1){
            fprintf(stderr, "Error reading from file %s\n", full_origin);
            free(full_destination);
            free(full_origin);
            free(buffer);
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // get file status of origin file
        struct stat origin_stat;
        if(stat(full_origin, &origin_stat) == -1){
            fprintf(stderr, "Error getting file status for %s\n", full_origin);
            free(full_destination);
            free(full_origin);
            free(buffer);
            close(fd1);
            close(fd2);
            exit(EXIT_FAILURE);
        }

        // set file status and modtime of destination file to be the same as origin file if -p option is specified
        if(find_option(option_list, 'p')){
            debug2 ==1 ? printf("st_mode: %d\n", origin_stat.st_mode) : 0;
            if(chmod(full_destination, origin_stat.st_mode) == -1){
                fprintf(stderr, "Error setting file permissions for %s\n", full_destination);
                free(full_destination);
                free(full_origin);
                free(buffer);
                close(fd1);
                close(fd2);
                exit(EXIT_FAILURE);
            }
            struct timeval times[2];
            times[0].tv_sec = time(NULL);
            times[0].tv_usec = 0;
            times[1].tv_sec = get_file(get_directory_from_hash_table(table, origin_dir), filename)->mtime;
            times[1].tv_usec = 0;
            debug2 == 1 ? printf("times[0].tv_sec: %ld, times[1].tv_sec: %ld\n", times[0].tv_sec, times[1].tv_sec) : 0;
            if(utimes(full_destination, times) == -1){
                fprintf(stderr, "Error setting file modtime for %s\n", full_destination);
                free(full_destination);
                free(full_origin);
                free(buffer);
                close(fd1);
                close(fd2);
                exit(EXIT_FAILURE);
            }
        }

        // close files
        if(close(fd1) == -1){
            fprintf(stderr, "Error closing file %s\n", full_origin);
            free(full_destination);
            free(full_origin);
            free(buffer);
            close(fd2);
            exit(EXIT_FAILURE);
        }
        if(close(fd2) == -1){
            fprintf(stderr, "Error closing file %s\n", full_destination);
            free(full_destination);
            free(full_origin);
            free(buffer);
            exit(EXIT_FAILURE);
        }

        // free memory
        free(buffer);
        free(full_destination);
        free(full_origin);
    }
    printf("\n");
}