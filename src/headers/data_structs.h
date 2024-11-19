#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include "base.h"

#include <getopt.h>
#include <locale.h>
#include <time.h>
#include <wchar.h>

// STRUCT DEFINITIONS

// FILE AND DIRECTORY STRUCTS

/**
 * @brief This struct represents a file
 *
 * (`fileName`, `filePath`, `fileSize`, `fileMtime`, `filePermissions`, `nextFile`)
 */
typedef struct File {
	char *fileName;			/**< The name of the file */
	char *filePath;			/**< The path of the file */
	off_t fileSize;			/**< The size of the file */
	time_t fileMtime;		/**< The modification time of the file */
	mode_t filePermissions; /**< The permissions of the file */
	struct File *nextFile;	/**< The next file in the list */
} File;

/**
 * @brief This struct represents a directory
 *
 * (`dirName`, `dirPath`, `parentDir`, `files`, `numFiles`, `headSubdir`, `tailSubdir`, `numSubdirs`, `nextDir`)
 */
typedef struct Directory {
	char *dirName;				  /**< The name of the directory */
	char *dirPath;				  /**< The absolute path of the directory from the provided directories */
	struct Directory *parentDir;  /**< The parent directory of the directory */
	File *files;				  /**< The list of files in the directory */
	int numFiles;				  /**< The number of files in the directory */
	struct Directory *headSubdir; /**< The head of the list of subdirectories */
	int numSubdirs;				  /**< The number of subdirectories in the directory */
	struct Directory *nextDir;	  /**< The next directory in the list */
} Directory;

/**
 * @brief This struct represents a list of directories
 *
 * (`head`, `tail`, `numDirs`)
 */
typedef struct DirectoryList {
	Directory *head; /**< The head of the list */
	Directory *tail; /**< The tail of the list */
	int numDirs;	 /**< The number of directories in the list */
} DirectoryList;

// COMMAND LINE OPTIONS STRUCTS

/**
 * @brief This struct represents a single command line option with its arguments if any
 *
 * (`flag`, `args`, `numArgs`, `nextOpt`)
 */
typedef struct Option {
	char flag;				/**< The flag of the option */
	char **args;			/**< The arguments of the option */
	int numArgs;			/**< The number of arguments of the option */
	struct Option *nextOpt; /**< The next option in the list */
} Option;

/**
 * @brief This struct represents a list of command line options
 *
 * (`head`, `tail`, `numOpts`)
 */
typedef struct OptionList {
	Option *head; /**< The head of the list */
	Option *tail; /**< The tail of the list */
	int numOpts;  /**< The number of options in the list */
} OptionList;

// MODIFICATION STRUCTS

/**
 * @brief This struct represents a modification to be made; i.e. a file to be copied
 *
 * (`fileSrcPath`, `srcDirPath`, `fileDestPath`, `nextMod`)
 */
typedef struct Modification {
	char *fileSrcPath;			  /**< The path of the file to be copied */
	char *srcDirPath;			  /**< The path of the source directory */
	char *fileDestPath;			  /**< The path to copy the file to */
	struct Modification *nextMod; /**< The next modification in the list */
} Modification;

/**
 * @brief This struct represents a list of modifications
 *
 * (`head`, `tail`, `numMods`)
 */
typedef struct ModificationList {
	Modification *head; /**< The head of the list */
	Modification *tail; /**< The tail of the list */
	int numMods;		/**< The number of modifications in the list */
} ModificationList;

// FUNCTION PROTOTYPES

// FILE AND DIRECTORY FUNCTION PROTOTYPES

/**
 * @brief This function initializes a new file
 *
 * @param fileName The name of the file
 * @param filePath The path of the file
 * @param fileSize The size of the file
 * @param fileMtime The modification time of the file
 * @param filePermissions The permissions of the file
 * @return A pointer to the new file if successful, `NULL` otherwise
 */
extern File *initFile(char *fileName, char *filePath, off_t fileSize, time_t fileMtime, mode_t filePermissions);

/**
 * @brief This function is a helper function that returns a string containing the details of a file
 *
 * @param file The file to get the details of
 * @return A string containing the details of the file if successful, `NULL` otherwise
 */
extern char *getFileDetails(File *file);

/**
 * @brief This function frees the memory allocated for a file
 *
 * @param file The file to free
 */
extern void freeFile(File *file);

/**
 * @brief This function initializes a new directory
 *
 * @param dirPath The path of the directory
 * @return A pointer to the new directory if successful, `NULL` otherwise
 */
extern Directory *initDirectory(char *dirPath);

/**
 * @brief This helper function prints the contents of a directory in a tree-like format
 *
 * @param dir The directory to print
 * @param depth The depth of the directory in the tree
 * @param isLast A boolean indicating if the directory is the last in the list
 */
extern void printDirectoryTree(Directory *dir, int depth, bool isLast);

/**
 * @brief This function uses `printDirectoryTree` to print the contents of a directory
 *
 * @param dir The directory to print
 */
extern void printDirectory(Directory *dir);

/**
 * @brief This function frees the memory allocated for a directory
 *
 * @param dir The directory to free
 */
extern void freeDirectory(Directory *dir);

/**
 * @brief This function adds a file to a directory
 *
 * @param dir The directory to add the file to
 * @param file The file to add
 */
extern void addFileToDirectory(Directory *dir, File *file);

/**
 * @brief This function checks if a file is in a directory
 *
 * @param dir The directory to check
 * @param file The file to check
 * @return `true` if the file is in the directory, `false` otherwise
 */
extern bool fileInDirectory(Directory *dir, File *file);

/**
 * @brief This function adds a subdirectory to a directory
 *
 * @param dir The directory to add the subdirectory to
 * @param subdir The subdirectory to add
 */
extern void addSubdirToDirectory(Directory *dir, Directory *subdir);

/**
 * @brief This function initializes a new directory list
 *
 * @return A pointer to the new directory list if successful, `NULL` otherwise
 */
extern DirectoryList *initDirectoryList();

/**
 * @brief This function prints the contents of a directory list
 *
 * @param dirList The directory list to print
 */
extern void printDirectoryList(DirectoryList *dirList);

/**
 * @brief This function frees the memory allocated for a directory list
 *
 * @param dirList The directory list to free
 */
extern void freeDirectoryList(DirectoryList *dirList);

/**
 * @brief This function adds a directory to a directory list
 *
 * @param dirList The directory list to add the directory to
 * @param dir The directory to add
 */
extern void addDirectoryToDirectoryList(DirectoryList *dirList, Directory *dir);

/**
 * @brief This function checks if a directory already exists in a directory list
 *
 * @param dirList The directory list to search
 * @param dir The directory to check
 * @return `true` if the directory is a duplicate, `false` otherwise
 */
extern bool isDuplicateDirectory(DirectoryList *dirList, Directory *dir);

// COMMAND LINE OPTIONS FUNCTION PROTOTYPES

/**
 * @brief This function initializes a list of command line options
 *
 * @return A pointer to the new option list if successful, `NULL` otherwise
 */
extern OptionList *initOptionList();

/**
 * @brief This function prints the contents of a list of command line options
 *
 * @param optList The option list to print
 */
extern void printOptionList(OptionList *optList);

/**
 * @brief This function frees the memory allocated for a list of command line options
 *
 * @param optList The option list to free
 */
extern void freeOptionList(OptionList *optList);

/**
 * @brief This function returns an option from a list of command line options
 *
 * @param optList The option list to search
 * @param flag The flag of the option to return
 * @return A pointer to the option if found, `NULL` otherwise
 */
extern Option *getOption(OptionList *optList, char flag);

/**
 * @brief This function adds an option to a list of command line options
 *
 * @param optList The option list to add the option to
 * @param flag The flag of the option to add
 * @param arg The argument of the option to add
 * @return A pointer to the new option if successful, `NULL` otherwise
 */
extern void addOptionToOptionList(OptionList *optList, char flag, char *arg);

// MODIFICATION FUNCTION PROTOTYPES

/**
 * @brief This function initializes a new modification
 *
 * @param fileSrcPath The path of the file to be copied
 * @param srcDirPath The path of the source directory
 * @param fileDestPath The path to where the file will be copied (including the file name)
 * @return A pointer to the new modification if successful, `NULL` otherwise
 */
extern Modification *initModification(char *fileSrcPath, char *srcDirPath, char *fileDestPath);

/**
 * @brief This function frees the memory allocated for a modification
 *
 * @param mod The modification to free
 */
extern void freeModification(Modification *mod);

/**
 * @brief This function initializes a new modification list
 *
 * @return A pointer to the new modification list if successful, `NULL` otherwise
 */
extern ModificationList *initModificationList();

/**
 * @brief This function prints the contents of a modification list
 *
 * @param modList The modification list to print
 */
extern void printModificationList(ModificationList *modList);

/**
 * @brief This function frees the memory allocated for a modification list
 *
 * @param modList The modification list to free
 */
extern void freeModificationList(ModificationList *modList);

/**
 * @brief This function adds a modification to a modification list
 *
 * @param modList The modification list to add the modification to
 * @param mod The modification to add
 */
extern void addModificationToModificationList(ModificationList *modList, Modification *mod);

#endif // DATA_STRUCTS_H