#ifndef COMPARISON_H
#define COMPARISON_H

#include "read_dir.h"

// FUNCTION PROTOTYPES

/**
 * @brief This function compares the modification times of two files
 *
 * @param file1 The first file to compare
 * @param file2 The second file to compare
 * @return The file with the most recent modification time if successful, `NULL` otherwise
 * @note If the modification times are equal, the first file is returned
 */
extern File *compareFilesMtime(File *file1, File *file2);

/**
 * @brief This function swaps the source and destination directories in a file path
 *
 * @param filePath The file path to modify
 * @param srcDirPath The source directory path
 * @param destDirPath The destination directory path
 * @return The modified file path if successful, `NULL` otherwise
 * @note The function allocates memory for the new file path and the caller is responsible for freeing it
 */
extern char *swapSrcDest(char *filePath, char *srcDirPath, char *destDirPath);

/**
 * @brief This function compares the files in two directories and returns a list of modifications needed to sync them
 *
 * @param dir1 The first directory to compare
 * @param dir2 The second directory to compare
 * @return A list of modifications needed to sync the directories if successful, `NULL` otherwise
 */
extern ModificationList *compareDirectories(Directory *dir1, Directory *dir2);

/**
 * @brief This function extends the `compareDirectories` function to allow for recursive directory comparison
 *
 * @param dir1 The first directory to compare
 * @param dir2 The second directory to compare
 * @return A list of modifications needed to sync the directories if successful, `NULL` otherwise
 */
extern ModificationList *compareDirectoriesRecursive(Directory *dir1, Directory *dir2);

/**
 * @brief This function compares all directories in a list with each other
 *
 * @param dirList The list of directories to compare
 * @return A list of modifications needed to sync all directories if successful, `NULL` otherwise
 */
extern ModificationList *compareAllDirectories(DirectoryList *dirList);

#endif // COMPARISON_H