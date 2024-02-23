#include "directorystruct.h"

// debug flag
int debug9 = 0;


// --------------------------------------------------------------------
// FUNCTION DEFINITIONS

directory* new_directory(char* name){
    directory* dir = malloc(sizeof(directory));
    CHECK_ALLOC(dir);
    dir->name = strdup(name);
    CHECK_ALLOC(dir->name);
    dir->children = malloc(sizeof(directory*));
    CHECK_ALLOC(dir->children);
    dir->num_children = 0;
    dir->files = malloc(sizeof(file*));
    CHECK_ALLOC(dir->files);
    dir->num_files = 0;
    return dir;
}

void print_directory(directory* dir){
    printf("Directory: %s\n", dir->name);
    printf("Children: ");
    for(int i = 0; i < dir->num_children; i++){
        printf("%s ", dir->children[i]->name);
    }
    printf("\n");
    printf("Files: ");
    for(int i = 0; i < dir->num_files; i++){
        char *timestr = ctime(&dir->files[i]->mtime);
        timestr[24] = '\0';
        printf("(%s, %s, %s) ", dir->files[i]->name, dir->files[i]->perms, timestr);
    }
    printf("\n\n");
}

void add_file(directory* dir, char* name){
    dir->files = realloc(dir->files, sizeof(file*) * (dir->num_files + 1));
    CHECK_ALLOC(dir->files);
    file* f = malloc(sizeof(file));
    CHECK_ALLOC(f);
    f->name = strdup(name);
    CHECK_ALLOC(f->name);
    f->mtime = 0;
    f->perms = "";
    dir->files[dir->num_files] = f;
    dir->num_files++;
}

bool file_in_directory(directory* dir, char* name){
    for(int i = 0; i < dir->num_files; i++){
        if(strcmp(dir->files[i]->name, name) == 0){
            return true;
        }
    }
    return false;
}

file* get_file(directory* dir, char* name){
    for(int i = 0; i < dir->num_files; i++){
        if(strcmp(dir->files[i]->name, name) == 0){
            return dir->files[i];
        }
    }
    return NULL;
}

void sub_directory(directory* dir, directory* child){
    dir->children = realloc(dir->children, sizeof(directory*) * (dir->num_children + 1));
    CHECK_ALLOC(dir->children);
    dir->children[dir->num_children] = child;
    dir->num_children++;
}

void free_directory(directory* dir) {
    // Free the memory for the name field
    free(dir->name);
    // Free the memory for the children array
    for(int i = 0; i < dir->num_children; i++) {
        free_directory(dir->children[i]);
    }
    free(dir->children);
    // Free the memory for the files array
    for(int i = 0; i < dir->num_files; i++){
        free(dir->files[i]->name);
        free(dir->files[i]);
    }
    free(dir->files);
    // Free the memory for the directory struct
    free(dir);
}

// ----------------------------------------------------------------

option_list* new_option_list(){
    option_list* list = malloc(sizeof(option_list));
    CHECK_ALLOC(list);
    list->options = malloc(sizeof(option_pattern*));
    CHECK_ALLOC(list->options);
    list->num_options = 0;
    return list;
}

void add_option(option_list* list, char flag, char* pattern){
    list->options = realloc(list->options, sizeof(option_pattern*) * (list->num_options + 1));
    CHECK_ALLOC(list->options);
    option_pattern* op = malloc(sizeof(option_pattern));
    CHECK_ALLOC(op);
    op->flag = flag;
    op->patterns = malloc(sizeof(char*));
    CHECK_ALLOC(op->patterns);
    op->patterns[0] = strdup(pattern);
    CHECK_ALLOC(op->patterns[0]);
    op->num_patterns = 1;
    list->options[list->num_options] = op;
    list->num_options++;
}

option_pattern* get_option(option_list* list, char flag){
    for(int i = 0; i < list->num_options; i++){
        if(list->options[i]->flag == flag){
            return list->options[i];
        }
    }
    return NULL;
}

bool find_option(option_list* list, char flag){
    for(int i = 0; i < list->num_options; i++){
        if(list->options[i]->flag == flag){
            return true;
        }
    }
    return false;
}

option_list* combine_options(option_list* list){
    option_list* combined = new_option_list();
    for(int i = 0; i < list->num_options; i++){
        debug9 == 1 ? printf("flag: %c, pattern: %s\n", list->options[i]->flag, list->options[i]->patterns[0]) : 0;
        if(!find_option(combined, list->options[i]->flag)){
            add_option(combined, list->options[i]->flag, list->options[i]->patterns[0]);
        }
        else{
            // add the pattern to the end of the patterns array for that option
            int index = 0;
            for(int j = 0; j < combined->num_options; j++){
                if(combined->options[j]->flag == list->options[i]->flag){
                    index = j;
                }
            }
            combined->options[index]->patterns = realloc(combined->options[index]->patterns, sizeof(char*) * (combined->options[index]->num_patterns + 1));
            CHECK_ALLOC(combined->options[index]->patterns);
            combined->options[index]->patterns[combined->options[index]->num_patterns] = strdup(list->options[i]->patterns[0]);
            CHECK_ALLOC(combined->options[index]->patterns[combined->options[index]->num_patterns]);
            combined->options[index]->num_patterns++;
        }
    }
    for(int i = 0; i < combined->num_options; i++){
        // if the option has no patterns, remove the empty pattern
        if(*(list->options[i]->patterns[0]) == '\0'){
            combined->options[i]->patterns = realloc(combined->options[i]->patterns, sizeof(char*));
            CHECK_ALLOC(combined->options[i]->patterns);
            combined->options[i]->num_patterns = 0;
        }
    }
    return combined;
}

void free_option_list(option_list* list){
    for(int i = 0; i < list->num_options; i++){
        for(int j = 0; j < list->options[i]->num_patterns; j++){
            free(list->options[i]->patterns[j]);
        }
        free(list->options[i]->patterns);
        free(list->options[i]);
    }
    free(list->options);
    free(list);
}