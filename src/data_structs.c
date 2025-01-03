#include "headers/data_structs.h"

// FUNCTION DEFINITIONS

// FILE AND DIRECTORY FUNCTION DEFINITIONS

File *initFile(char *fileName, char *filePath, off_t fileSize, time_t fileMtime, mode_t filePermissions)
{
	File *file = calloc(1, sizeof(File));
	CHECK_ALLOC(file);
	file->fileName = strdup(fileName);
	CHECK_ALLOC(file->fileName);
	file->filePath = strdup(filePath);
	CHECK_ALLOC(file->filePath);
	file->fileSize = fileSize;
	file->fileMtime = fileMtime;
	file->filePermissions = filePermissions;
	file->nextFile = NULL;
	return file;
}

char *getFileDetails(File *file)
{
	char *permStr = perm2str(file->filePermissions);
	if (permStr == NULL) {
		perror(__func__);
		return NULL;
	}
	char *mtimeStr = ctime(&file->fileMtime);
	if (mtimeStr == NULL) {
		perror(__func__);
		return NULL;
	}
	mtimeStr[strlen(mtimeStr) - 1] = '\0'; // Remove the newline character
	char *details = calloc(strlen(file->fileName) + strlen(permStr) + strlen(mtimeStr) + 100, sizeof(char));
	if (details == NULL) {
		perror(__func__);
		return NULL;
	}
	sprintf(details, "%s \t\t [size={%ld bytes}, perms={%s}, mtime={%s}]", file->fileName, file->fileSize, permStr, mtimeStr);
	free(permStr);
	return details;
}

void freeFile(File *file)
{
	if (file == NULL) { return; }
	free(file->fileName);
	free(file->filePath);
	free(file);
}

Directory *initDirectory(char *dirPath)
{
	Directory *dir = calloc(1, sizeof(Directory));
	CHECK_ALLOC(dir);

	// Extract the directory name from the path
	char *dirName = strrchr(dirPath, '/');
	if (dirName == NULL) {
		// No '/' character found, so the directory is the root directory
		dir->dirName = strdup(dirPath);
		CHECK_ALLOC(dir->dirName);
	} else {
		// '/' character found, so the directory name is the substring after the last '/'
		dir->dirName = strdup(dirName + 1);
		CHECK_ALLOC(dir->dirName);
	}
	// Ensure the directory name ends with a '/'
	size_t len = strlen(dir->dirName);
	if (dir->dirName[len - 1] != '/') {
		char *temp = calloc(len + 2, sizeof(char)); // '/' and '\0'
		CHECK_ALLOC(temp);
		strcpy(temp, dir->dirName);
		strcat(temp, "/");
		free(dir->dirName);
		dir->dirName = temp;
	}

	dir->dirPath = strdup(dirPath);
	CHECK_ALLOC(dir->dirPath);
	dir->parentDir = NULL;
	dir->files = NULL;
	dir->numFiles = 0;
	dir->headSubdir = NULL;
	dir->numSubdirs = 0;
	dir->nextDir = NULL;
	return dir;
}

void printDirectoryTree(Directory *dir, int depth, bool isLast)
{
	setlocale(LC_CTYPE, "");
	wchar_t branchChar = 0x251C;	 // '├'
	wchar_t lastBranchChar = 0x2514; // '└'
	wchar_t verticalChar = 0x2502;	 // '│'
	wchar_t horizontalChar = 0x2500; // '─'
	wchar_t spaceChar = 0x0020;		 // ' '

	if (dir == NULL) return;
	if (depth > 0) {
		printf("\033[1;34m\t%lc%lc%lc %s\t (%d f, %d s)\033[0m\n",
			   isLast ? lastBranchChar : branchChar,
			   horizontalChar,
			   horizontalChar,
			   dir->dirName,
			   dir->numFiles,
			   dir->numSubdirs);
	} else {
		printf("\033[1;34m\t%s\033[0m\n", dir->dirName);
	}

	if (dir->numFiles == 0 && dir->numSubdirs == 0) { return; }

	int totalItems = dir->numFiles + dir->numSubdirs;
	int currentItem = 0;

	File *file = dir->files;
	while (file != NULL) {
		currentItem++;
		for (int i = 0; i < depth; i++) { printf("\t"); }

		bool isLastItem = (currentItem == totalItems);
		char *fileDetailsStr = getFileDetails(file);
		if (fileDetailsStr == NULL) {
			perror(__func__);
			return;
		}
		printf("\033[1;32m\t%lc%lc%lc %s\033[0m\n", isLastItem ? lastBranchChar : branchChar, horizontalChar, horizontalChar, fileDetailsStr);
		free(fileDetailsStr);
		file = file->nextFile;
	}

	Directory *subdir = dir->headSubdir;
	while (subdir != NULL) {
		currentItem++;
		for (int i = 0; i < depth; i++) {
			printf("\033[\t%lc\033[0m", (isLast && currentItem == totalItems) ? spaceChar : verticalChar);
		}
		bool isLastItem = (currentItem == totalItems);
		printDirectoryTree(subdir, depth + 1, isLastItem);
		subdir = subdir->nextDir;
	}
}

void printDirectory(Directory *dir)
{
	printf("----------------------------------------------------------------------------------------------------\n");
	printf("\033[1;35m%s (%d file%s, %d subdirector%s)\033[0m\n\n",
		   dir->dirPath,
		   dir->numFiles,
		   dir->numFiles == 1 ? "" : "s",
		   dir->numSubdirs,
		   dir->numSubdirs == 1 ? "y" : "ies");
	printDirectoryTree(dir, 0, true);
	printf("\n");
}

void freeDirectory(Directory *dir)
{
	if (dir == NULL) { return; }
	free(dir->dirName);
	free(dir->dirPath);
	File *file = dir->files;
	while (file != NULL) {
		File *temp = file;
		file = file->nextFile;
		freeFile(temp);
	}
	Directory *subdir = dir->headSubdir;
	while (subdir != NULL) {
		Directory *temp = subdir;
		subdir = subdir->nextDir;
		freeDirectory(temp);
	}
	free(dir);
}

void addFileToDirectory(Directory *dir, File *file)
{
	if (dir == NULL || file == NULL) { return; }
	// LIFO order
	file->nextFile = dir->files;
	dir->files = file;
	dir->numFiles++;
}

void addSubdirToDirectory(Directory *dir, Directory *subdir)
{
	if (dir == NULL || subdir == NULL) { return; }
	subdir->parentDir = dir;
	// LIFO order
	subdir->nextDir = dir->headSubdir;
	dir->headSubdir = subdir;
	dir->numSubdirs++;
}

DirectoryList *initDirectoryList()
{
	DirectoryList *dirList = calloc(1, sizeof(DirectoryList));
	CHECK_ALLOC(dirList);
	dirList->head = NULL;
	dirList->tail = NULL;
	dirList->numDirs = 0;
	return dirList;
}

void printDirectoryList(DirectoryList *dirList, int status)
{
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\033[1;31mDIRECTORY LIST %s SYNC\033[0m (%d unique director%s)\n\n",
		   status == 0 ? "BEFORE" : "AFTER",
		   dirList->numDirs,
		   dirList->numDirs == 1 ? "y" : "ies");
	Directory *dir = dirList->head;
	while (dir != NULL) {
		printDirectory(dir);
		dir = dir->nextDir;
	}
	printf("----------------------------------------------------------------------------------------------------\n\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END ~~~~~~ OF ~~~~~~ DIRECTORYLIST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void freeDirectoryList(DirectoryList *dirList)
{
	if (dirList == NULL) { return; }
	Directory *dir = dirList->head;
	while (dir != NULL) {
		Directory *temp = dir;
		dir = dir->nextDir;
		freeDirectory(temp);
	}
	free(dirList);
}

void addDirectoryToDirectoryList(DirectoryList *dirList, Directory *dir)
{
	if (dirList == NULL || dir == NULL) { return; }
	if (isDuplicateDirectory(dirList, dir)) {
		freeDirectory(dir);
		return;
	}
	if (dirList->head == NULL) {
		dirList->head = dir;
		dirList->tail = dir;
	} else {
		dirList->tail->nextDir = dir;
		dirList->tail = dir;
	}
	dirList->numDirs++;
}

bool isDuplicateDirectory(DirectoryList *dirList, Directory *dir)
{
	if (dirList != NULL && dir != NULL) {
		Directory *temp = dirList->head;
		while (temp != NULL) {
			if (strcmp(temp->dirPath, dir->dirPath) == 0) { return true; }
			temp = temp->nextDir;
		}
	}
	return false;
}

// COMMAND LINE OPTIONS FUNCTION DEFINITIONS

OptionList *initOptionList()
{
	OptionList *optList = calloc(1, sizeof(OptionList));
	CHECK_ALLOC(optList);
	optList->head = NULL;
	optList->tail = NULL;
	optList->numOpts = 0;
	return optList;
}

void printOptionList(OptionList *optList)
{
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\033[1;31mOPTION LIST\033[0m (%d option%s)\n\n", optList->numOpts, optList->numOpts == 1 ? "" : "s");
	Option *opt = optList->head;
	while (opt != NULL) {
		if (opt->numArgs == 0) {
			printf("\t -%c", opt->flag);
		} else if (opt->numArgs == 1) {
			printf("\t -%c (1 arg): ", opt->flag);
		} else {
			printf("\t -%c (%d args): ", opt->flag, opt->numArgs);
		}
		for (int i = 0; i < opt->numArgs; i++) { printf("\"%s\"%s", opt->args[i], i < opt->numArgs - 1 ? ", " : ""); }
		printf("\n");
		opt = opt->nextOpt;
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END ~~~~~~ OF ~~~~~~ OPTIONLIST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void freeOptionList(OptionList *optList)
{
	if (optList == NULL) { return; }
	Option *opt = optList->head;
	while (opt != NULL) {
		Option *temp = opt;
		opt = opt->nextOpt;
		for (int i = 0; i < temp->numArgs; i++) { free(temp->args[i]); }
		free(temp->args);
		free(temp);
	}
	free(optList);
}

Option *getOption(OptionList *optList, char flag)
{
	if (optList != NULL) {
		Option *opt = optList->head;
		while (opt != NULL) {
			if (opt->flag == flag) { return opt; }
			opt = opt->nextOpt;
		}
	}
	return NULL;
}

void addOptionToOptionList(OptionList *optList, char flag, char *arg)
{
	if (optList == NULL) { return; }
	// if option already exists, add arg to the args array; if no arg and option exists, skip
	Option *opt = getOption(optList, flag);
	if (opt != NULL) {
		if (arg != NULL) {
			opt->args = realloc(opt->args, (opt->numArgs + 1) * sizeof(char *));
			CHECK_ALLOC(opt->args);
			opt->args[opt->numArgs] = strdup(arg);
			CHECK_ALLOC(opt->args[opt->numArgs]);
			opt->numArgs++;
		}
	}
	// if option does not exist, create new option and add to list
	else {
		opt = calloc(1, sizeof(Option));
		CHECK_ALLOC(opt);
		opt->flag = flag;
		if (arg != NULL) {
			opt->args = calloc(1, sizeof(char *));
			CHECK_ALLOC(opt->args);
			opt->args[0] = strdup(arg);
			CHECK_ALLOC(opt->args[0]);
			opt->numArgs = 1;
		} else {
			opt->args = NULL;
			opt->numArgs = 0;
		}
		if (optList->head == NULL) {
			optList->head = opt;
			optList->tail = opt;
		} else {
			optList->tail->nextOpt = opt;
			optList->tail = opt;
		}
		optList->numOpts++;
	}
}

// MODIFICATION FUNCTION DEFINITIONS

Modification *initModification(char *fileSrcPath, char *srcDirPath, char *fileDestPath)
{
	Modification *mod = calloc(1, sizeof(Modification));
	CHECK_ALLOC(mod);
	mod->fileSrcPath = strdup(fileSrcPath);
	CHECK_ALLOC(mod->fileSrcPath);
	mod->srcDirPath = strdup(srcDirPath);
	CHECK_ALLOC(mod->srcDirPath);
	mod->fileDestPath = strdup(fileDestPath);
	CHECK_ALLOC(mod->fileDestPath);
	mod->nextMod = NULL;
	return mod;
}

void freeModification(Modification *mod)
{
	if (mod == NULL) { return; }
	free(mod->fileSrcPath);
	free(mod->srcDirPath);
	free(mod->fileDestPath);
	free(mod);
}

ModificationList *initModificationList()
{
	ModificationList *modList = calloc(1, sizeof(ModificationList));
	CHECK_ALLOC(modList);
	modList->head = NULL;
	modList->tail = NULL;
	modList->numMods = 0;
	return modList;
}

void printModificationList(ModificationList *modList)
{
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\033[1;31mMODIFICATION LIST\033[0m (%d modification%s needed):\n\n", modList->numMods, modList->numMods == 1 ? "" : "s");
	modList->numMods > 0 ? printf("%-50s %-50s\n", "FILE SRC PATH", "FILE DEST PATH") : printf("No modifications needed\n");
	Modification *mod = modList->head;
	while (mod != NULL) {
		printf("\033[1;32m%-50s %-50s\033[0m\n", mod->fileSrcPath, mod->fileDestPath);
		mod = mod->nextMod;
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END ~~~~~~ OF ~~~~~~ MODIFICATIONLIST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void freeModificationList(ModificationList *modList)
{
	if (modList == NULL) { return; }
	Modification *mod = modList->head;
	while (mod != NULL) {
		Modification *temp = mod;
		mod = mod->nextMod;
		freeModification(temp);
	}
	free(modList);
}

void addModificationToModificationList(ModificationList *modList, Modification *mod)
{
	if (modList == NULL || mod == NULL) { return; }
	if (modList->head == NULL) {
		modList->head = mod;
		modList->tail = mod;
	} else {
		modList->tail->nextMod = mod;
		modList->tail = mod;
	}
	modList->numMods++;
}