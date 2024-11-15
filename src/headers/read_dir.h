#ifndef READ_DIR_H
#define READ_DIR_H

#include "data_structs.h"

#include <dirent.h>
#include <fcntl.h>
#include <regex.h>
#include <sys/stat.h>

// FUNCTION PROTOTYPES

/**
 * @brief This function checks if a file is hidden (starts with a dot '.' character)
 *
 * @param filename The name of the file
 * @return `true` if the file is hidden, `false` otherwise
 */
extern bool isHidden(char *filename);

/**
 * @brief This function reads the contents of a single directory
 *
 * @param dirPath The path of the directory to read
 * @param optList The list of command line options
 * @return A pointer to the directory if successful, `NULL` otherwise
 */
extern Directory *readDirectory(char *dirPath, OptionList *optList);

/**
 * @brief This function reads the contents of all provided directories adding them to a list
 *
 * @param argc The number of command line arguments
 * @param optind The index of the first directory in the array of command line arguments
 * @param argv The array of command line arguments
 * @param optList The list of command line options
 * @return A pointer to the list of directories if successful, `NULL` otherwise
 */
extern DirectoryList *readDirectories(int argc, int optind, char *argv[], OptionList *optList);

#endif // READ_DIR_H