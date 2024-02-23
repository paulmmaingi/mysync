#include "file_difference.h"


// --------------------------------------------------------------------
// FUNCTION DECLARATIONS

file* compare_mtime(file* file1, file* file2){
    if (file1->mtime > file2->mtime){
        return file1;
    }
    else{
        return NULL;
    }
}

mod_tup* new_mod_tup(char* filepath, char* origin_dir, char* destination_dir){
    mod_tup* mod_tup = malloc(sizeof(mod_tup));
    CHECK_ALLOC(mod_tup);
    mod_tup->filepath = strdup(filepath);
    CHECK_ALLOC(mod_tup->filepath);
    mod_tup->origin_dir = strdup(origin_dir);
    CHECK_ALLOC(mod_tup->origin_dir);   
    mod_tup->destination_dir = strdup(destination_dir);
    CHECK_ALLOC(mod_tup->destination_dir);
    return mod_tup;
}

mod_tup_list* new_mod_tup_list(void){
    mod_tup_list* list = malloc(sizeof(mod_tup_list));
    CHECK_ALLOC(list);
    list->mod_tups = malloc(sizeof(mod_tup*));
    CHECK_ALLOC(list->mod_tups);
    list->num_mod_tups = 0;
    return list;
}

void add_mod_tup(mod_tup_list* list, mod_tup* current){
    list->mod_tups = realloc(list->mod_tups, sizeof(mod_tup*) * (list->num_mod_tups + 1));
    CHECK_ALLOC(list->mod_tups);
    list->mod_tups[list->num_mod_tups] = current;
    list->num_mod_tups++;
}

void free_mod_tup_list(mod_tup_list* list){
    for (int i = 0; i < list->num_mod_tups; i++){
        free(list->mod_tups[i]->filepath);
        free(list->mod_tups[i]->origin_dir);
        free(list->mod_tups[i]->destination_dir);
        free(list->mod_tups[i]);
    }
    free(list->mod_tups);
    free(list);
}

void print_mod_tup_list(mod_tup_list* list){
    list->num_mod_tups != 0 ? printf("\nModifications needed List:\n\n") : printf("\nNo modifications needed\n");
    for (int i = 0; i < list->num_mod_tups; i++){
        // if can fit in one line
        if(strlen(list->mod_tups[i]->filepath) + strlen(list->mod_tups[i]->origin_dir) + strlen(list->mod_tups[i]->destination_dir) < 80){
            printf("file: %-30s origin: %-30s destination: %-30s\n", list->mod_tups[i]->filepath, list->mod_tups[i]->origin_dir, list->mod_tups[i]->destination_dir);
        }
        // if cannot fit in one line
        else{
            printf("file: %-30s\norigin: %-30s\ndestination: %-30s\n\n", list->mod_tups[i]->filepath, list->mod_tups[i]->origin_dir, list->mod_tups[i]->destination_dir);
        }
    }
    printf("\n");
}

mod_tup_list* create_mini_mod_tup_list(directory* dir1, directory* dir2, option_list* option_list, hash_table* table){
    // make new modification tuple list
    mod_tup_list* list = new_mod_tup_list();

    // iterate through files in dir1 and check if they are in dir2
    for (int i = 0; i < dir1->num_files; i++){
        if(file_in_directory(dir2, dir1->files[i]->name)){
            file* latest = compare_mtime(dir1->files[i], get_file(dir2, dir1->files[i]->name));
            if(latest == NULL){
                continue;
            }
            if(latest != NULL){
                mod_tup* temp = new_mod_tup(latest->name, dir1->name, dir2->name);
                add_mod_tup(list, temp);
            }
            // printf("filepath: %s, origin: %s , destination: %s, time: %s\n", filepath, dir1->name, dir2->name,ctime(&get_file(dir2, dir1->files[i]->name)->mtime));
        }
        // if the file is not in dir2, add it to the modification tuple list
        else{
            mod_tup* temp = new_mod_tup(dir1->files[i]->name, dir1->name, dir2->name);
            add_mod_tup(list, temp);
        }
    }
    // iterate through files in dir2 and check if they are in dir1
    for (int i = 0; i < dir2->num_files; i++){
        if(file_in_directory(dir1, dir2->files[i]->name)){
            file* latest = compare_mtime(dir2->files[i], get_file(dir1, dir2->files[i]->name));
            if(latest == NULL){
                continue;
            }
            if(latest != NULL){
                mod_tup* temp = new_mod_tup(latest->name, dir2->name, dir1->name);
                add_mod_tup(list, temp);
            }
            // printf("filepath: %s, origin: %s , destination: %s, time: %s\n", filepath, dir2->name, dir1->name,ctime(&get_file(dir1, dir2->files[i]->name)->mtime));
        }
        // if the file is not in dir1, add it to the modification tuple list
        else{
            mod_tup* temp = new_mod_tup(dir2->files[i]->name, dir2->name, dir1->name);
            add_mod_tup(list, temp);
        }
    }

    // if -r is in the option list, create the subdirectories in the destination directory and add the modification tuple lists for those subdirectories to the modification tuple list
    if(find_option(option_list, 'r')){
        for(int i = 0; i < dir1->num_children; i++){
            char* origin_dir_name = malloc(sizeof(char) * (strlen(dir1->name) + strlen(dir1->children[i]->name) + 2));
            CHECK_ALLOC(origin_dir_name);
            strcpy(origin_dir_name, dir1->name);
            strcat(origin_dir_name, "/");
            strcat(origin_dir_name, dir1->children[i]->name);
            directory* origin_dir = get_directory_from_hash_table(table, origin_dir_name);
            free(origin_dir_name);

            char* destination_dir_name = malloc(sizeof(char) * (strlen(dir2->name) + strlen(dir1->children[i]->name) + 2));
            CHECK_ALLOC(destination_dir_name);
            strcpy(destination_dir_name, dir2->name);
            strcat(destination_dir_name, "/");
            strcat(destination_dir_name, dir1->children[i]->name);
            
            // if the destination directory does not exist, create new dir struct for it and add it to the hash table and modtuplist (not creating the directory yet)
            if(get_directory_from_hash_table(table, destination_dir_name) == NULL){
                directory* destination_dir = new_directory(destination_dir_name);
                add_directory_to_hash_table(table, destination_dir);
                mod_tup_list* sub_list = create_mini_mod_tup_list(origin_dir, destination_dir, option_list, table);
                union_mod_tup_lists(list, sub_list);
            }
            // if the destination directory exists, add the modification tuple list for it to the modification tuple list
            else{
                mod_tup_list* sub_list = create_mini_mod_tup_list(origin_dir, get_directory_from_hash_table(table, destination_dir_name), option_list, table);
                union_mod_tup_lists(list, sub_list);
            }
            // free memory
            free(destination_dir_name);
        }


        for(int i = 0; i < dir2->num_children; i++){
            char* origin_dir_name = malloc(sizeof(char) * (strlen(dir2->name) + strlen(dir2->children[i]->name) + 2));
            CHECK_ALLOC(origin_dir_name);
            strcpy(origin_dir_name, dir2->name);
            strcat(origin_dir_name, "/");
            strcat(origin_dir_name, dir2->children[i]->name);
            directory* origin_dir = get_directory_from_hash_table(table, origin_dir_name);
            free(origin_dir_name);

            char* destination_dir_name = malloc(sizeof(char) * (strlen(dir1->name) + strlen(dir2->children[i]->name) + 2));
            CHECK_ALLOC(destination_dir_name);
            strcpy(destination_dir_name, dir1->name);
            strcat(destination_dir_name, "/");
            strcat(destination_dir_name, dir2->children[i]->name);
            
            // if the destination directory does not exist, create new dir struct for it and add it to the hash table and modtuplist (not creating the directory yet)
            if(get_directory_from_hash_table(table, destination_dir_name) == NULL){
                directory* destination_dir = new_directory(destination_dir_name);
                add_directory_to_hash_table(table, destination_dir);
                mod_tup_list* sub_list = create_mini_mod_tup_list(origin_dir, destination_dir, option_list, table);
                union_mod_tup_lists(list, sub_list);
            }
            // if the destination directory exists, add the modification tuple list for it to the modification tuple list
            else{
                mod_tup_list* sub_list = create_mini_mod_tup_list(origin_dir, get_directory_from_hash_table(table, destination_dir_name), option_list, table);
                union_mod_tup_lists(list, sub_list);
            }
            // free memory
            free(destination_dir_name);
        }
    }
    return list;
}

mod_tup_list* union_mod_tup_lists(mod_tup_list* list1, mod_tup_list* list2){
    // add only if the modification tuple is not already in the list
    for (int i = 0; i < list2->num_mod_tups; i++){
        bool add = true;
        for (int j = 0; j < list1->num_mod_tups; j++){
            if(strcmp(list2->mod_tups[i]->filepath, list1->mod_tups[j]->filepath) == 0 && strcmp(list2->mod_tups[i]->origin_dir, list1->mod_tups[j]->origin_dir) == 0 && strcmp(list2->mod_tups[i]->destination_dir, list1->mod_tups[j]->destination_dir) == 0){
                add = false;
                break;
            }
        }
        if(add){
            add_mod_tup(list1, list2->mod_tups[i]);
        }
    }
    free(list2->mod_tups);
    free(list2);
    return list1;
}

mod_tup_list* update_entire_mod_tup_list(hash_table* table, mod_tup_list* session_mod_tup_list, int ndirs, char** argv, option_list* option_list){
    for (int i = 0; i < ndirs; i++){
        for (int j = i + 1; j < ndirs; j++){
            mod_tup_list* list = create_mini_mod_tup_list(get_directory_from_hash_table(table, argv[i]), get_directory_from_hash_table(table, argv[j]), option_list, table);
            session_mod_tup_list = union_mod_tup_lists(session_mod_tup_list, list);
        }
    }
    return session_mod_tup_list;
}