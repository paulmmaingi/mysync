#include "directorystruct.h"
#include <unistd.h>

// size of hash table
#define	HASHTABLE_SIZE		997


// --------------------------------------------------------------------
// DATA STRUCTURES

// Define a struct for a hash table entry (key -> from hashfunction, value -> directory*, next hash entry pointer)
typedef struct hash_entry {
    char* key;
    directory* value;
    struct hash_entry* next;
} hash_entry;

// Define a struct for a hash table (size, table -> array of hash_entry*, each hash_entry* is a linked list)
typedef struct hash_table {
    int size;
    hash_entry** table;
} hash_table;


// --------------------------------------------------------------------
// FUNCTION DECLARATIONS

// Creates a new hash table with the given size and returns a pointer to it
extern hash_table* new_hash_table(int size);

// Hash function for directory names, returns the hash value
extern int hash_directory_name(char* name, int size);

// Adds a directory to the hash table
extern void add_directory_to_hash_table(hash_table* table, directory* dir);

// Gets a directory from the hash table given its name
extern directory* get_directory_from_hash_table(hash_table* table, char* name);

// Prints the contents of the hash table
extern void print_hash_table(hash_table* table);

// Frees the memory used by the hash table
extern void free_hash_table(hash_table* table);