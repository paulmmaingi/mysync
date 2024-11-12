#ifndef READ_DIR_H
#define READ_DIR_H

#include "data_structs.h"

#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>

// FUNCTION PROTOTYPES

/**
 * @brief This function checks if a file is hidden (starts with a dot '.' character)
 *
 * @param filename The name of the file
 * @return `true` if the file is hidden, `false` otherwise
 */
extern bool isHidden(char *filename);

#endif // READ_DIR_H