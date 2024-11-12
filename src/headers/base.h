#ifndef BASE_H
#define BASE_H

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// SOME MACROS AND CONSTANTS

/**
 * @brief This macro checks if a pointer is NULL and exits the program if it is.
 *
 * @param ptr The pointer to check
 */
#define CHECK_ALLOC(ptr)                                                                                               \
	if (ptr == NULL) {                                                                                                 \
		perror(__func__);                                                                                              \
		exit(1);                                                                                                       \
	}

// FUNCTION PROTOTYPES

/**
 * @brief This function converts a glob pattern to a regex pattern.
 *
 * @param glob The glob pattern to convert to regex
 * @return The regex pattern if the conversion was successful, `NULL` otherwise
 * @note The function allocates memory for the regex pattern and the caller is responsible for freeing it.
 */
extern char *glob2regex(char *glob);

/**
 * @brief This function converts the permissions of a file to a string representation (human-readable format) e.g. rwxr-xr--.
 *
 * @param mode The permissions of the file
 * @return The string representation of the permissions if the conversion was successful, `NULL` otherwise
 * @note The function allocates memory for the string representation and the caller is responsible for freeing it.
 */
extern char *perm2str(mode_t mode);

#endif // BASE_H