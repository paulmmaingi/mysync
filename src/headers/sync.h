#ifndef SYNC_H
#define SYNC_H

#include "comparison.h"

#include <sys/time.h>

// FUNCTION PROTOTYPES

/**
 * @brief This function creates intermediate directories for a file path so that the directory exists when the file is to be copied
 *
 * @param modList The modification list containing the target file paths
 */
extern void createIntermediateDirectories(ModificationList *modList, bool verbose);

/**
 * @brief This function copies a file from the source path to the destination path
 *
 * @param srcPath The path of the source file
 * @param destPath The path of the destination file
 * @param optList The list of command line options
 */
extern void copyFile(char *srcPath, char *destPath, OptionList *optList);

/**
 * @brief This function performs the sync operation by processing the modification list and doing the necessary copying
 *
 * @param modList The list of modifications to be made
 * @param optList The list of command line options
 */
extern void syncFiles(ModificationList *modList, OptionList *optList);

#endif // SYNC_H