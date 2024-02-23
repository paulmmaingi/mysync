#include "hashtable.h"


// --------------------------------------------------------------------
// FUNCTION DEFINITIONS

hash_table* new_hash_table(int size) {
    // Allocate memory for the hash table
    hash_table* table = (hash_table*) malloc(sizeof(hash_table));
    CHECK_ALLOC(table);
    // Allocate memory for the table array
    table->table = (hash_entry**) calloc(size, sizeof(hash_entry*));
    CHECK_ALLOC(table->table);
    // Set the size of the hash table
    table->size = size;
    return table;
}

int hash_directory_name(char* name, int size) {
    int hash = 0;
    for (int i = 0; i < strlen(name); i++) {
        hash = (hash * 31 + name[i]) % size;
    }
    return hash;
}

void add_directory_to_hash_table(hash_table* table, directory* dir) {
    // Compute the hash value for the directory name
    int hash = hash_directory_name(dir->name, table->size);
    
    // Create a new hash table entry for the directory
    hash_entry* entry = (hash_entry*) malloc(sizeof(hash_entry));
    CHECK_ALLOC(entry);
    entry->key = strdup(dir->name);
    CHECK_ALLOC(entry->key);
    entry->value = dir;
    entry->next = NULL;

    // Add the entry to the hash table
    if (table->table[hash] == NULL) {
        table->table[hash] = entry;
    } 
    else {
        hash_entry* curr = table->table[hash];
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = entry;
    }
}

directory* get_directory_from_hash_table(hash_table* table, char* name) {
    // Compute the hash value for the directory name
    int hash = hash_directory_name(name, table->size);

    // Search for the directory in the hash table
    hash_entry* curr = table->table[hash];
    while (curr != NULL) {
        if (strcmp(curr->key, name) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

void print_hash_table(hash_table* table) {
    for (int i = 0; i < table->size; i++) {
        if (table->table[i] != NULL) {
            hash_entry* curr = table->table[i];
            while (curr != NULL) {
                print_directory(curr->value);
                curr = curr->next;
            }
        }
    }
}

void free_hash_table(hash_table* table) {
    for (int i = 0; i < table->size; i++) {
        if (table->table[i] != NULL) {
            hash_entry* curr = table->table[i];
            while (curr != NULL) {
                free(curr->key);
                curr = curr->next;
            }
        }
    }
    free(table->table);
    free(table);
}