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

void printFile(File *file)
{
	char *permStr = perm2str(file->filePermissions);
	if (permStr == NULL) {
		perror(__func__);
		return;
	}
	char *mtimeStr = ctime(&file->fileMtime);
	if (mtimeStr == NULL) {
		perror(__func__);
		return;
	}
	mtimeStr[strlen(mtimeStr) - 1] = '\0'; // Remove the newline character
	printf("\t  (%s [%s]): size=%ld bytes, mtime=%s\n, perms=%s\n", file->fileName, file->filePath, file->fileSize, mtimeStr, permStr);
	free(permStr);
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
	dir->dirPath = strdup(dirPath);
	CHECK_ALLOC(dir->dirPath);
	dir->parentDir = NULL;
	dir->files = NULL;
	dir->numFiles = 0;
	dir->subdirs = NULL;
	dir->numSubdirs = 0;
	dir->nextDir = NULL;
	return dir;
}

void printDirectory(Directory *dir)
{
	printf("----------------------------------------------------------------------------------------------------\n");
	printf("DIRECTORY %s (%d file%s, %d subdirector%s)\n\n",
		   dir->dirPath,
		   dir->numFiles,
		   dir->numFiles == 1 ? "" : "s",
		   dir->numSubdirs,
		   dir->numSubdirs == 1 ? "y" : "ies");
	File *file = dir->files;
	while (file != NULL) {
		printFile(file);
		file = file->nextFile;
	}
	Directory **subdir = dir->subdirs;
	for (int i = 0; i < dir->numSubdirs; i++) { printDirectory(subdir[i]); }
	printf("\n");
}

void freeDirectory(Directory *dir)
{
	if (dir == NULL) { return; }
	free(dir->dirPath);
	File *file = dir->files;
	while (file != NULL) {
		File *temp = file;
		file = file->nextFile;
		freeFile(temp);
	}
	if (dir->subdirs != NULL) {
		for (int i = 0; i < dir->numSubdirs; i++) { freeDirectory(dir->subdirs[i]); }
		free(dir->subdirs);
	}
	free(dir);
}

void addFileToDirectory(Directory *dir, File *file)
{
	if (dir == NULL || file == NULL) { return; }
	file->nextFile = dir->files;
	dir->files = file;
	dir->numFiles++;
}

bool fileInDirectory(Directory *dir, File *file)
{
	if (dir != NULL && file != NULL) {
		File *temp = dir->files;
		while (temp != NULL) {
			if (strcmp(temp->fileName, file->fileName) == 0) { return true; }
			temp = temp->nextFile;
		}
	}
	return false;
}

void addSubdirToDirectory(Directory *dir, Directory *subdir)
{
	if (dir == NULL || subdir == NULL) { return; }
	subdir->parentDir = dir;
	if (dir->subdirs == NULL) {
		dir->subdirs = calloc(1, sizeof(Directory *));
		CHECK_ALLOC(dir->subdirs);
		dir->subdirs[0] = subdir;
	} else {
		dir->subdirs = realloc(dir->subdirs, (dir->numSubdirs + 1) * sizeof(Directory *));
		CHECK_ALLOC(dir->subdirs);
		dir->subdirs[dir->numSubdirs] = subdir;
	}
	if (dir->numSubdirs > 0) { dir->subdirs[dir->numSubdirs - 1]->nextDir = subdir; }
	subdir->nextDir = NULL;
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

void printDirectoryList(DirectoryList *dirList)
{
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("DIRECTORY LIST (%d director%s)\n\n", dirList->numDirs, dirList->numDirs == 1 ? "y" : "ies");
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
	if (dirList->head == NULL) {
		dirList->head = dir;
		dirList->tail = dir;
	} else {
		dirList->tail->nextDir = dir;
		dirList->tail = dir;
	}
	dirList->numDirs++;
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
	printf("OPTION LIST (%d option%s)\n\n", optList->numOpts, optList->numOpts == 1 ? "" : "s");
	Option *opt = optList->head;
	while (opt != NULL) {
		printf("\t -%c%s%s%d%s", opt->flag, opt->numArgs > 0 ? " " : "", opt->numArgs > 0 ? "(" : "", opt->numArgs, opt->numArgs > 0 ? " args)" : "");
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

Modification *initModification(char *filePath, char *srcDirPath, char *destDirPath)
{
	Modification *mod = calloc(1, sizeof(Modification));
	CHECK_ALLOC(mod);
	mod->filePath = strdup(filePath);
	CHECK_ALLOC(mod->filePath);
	mod->srcDirPath = strdup(srcDirPath);
	CHECK_ALLOC(mod->srcDirPath);
	// NEED TO MODIFY DESTDIRPATH TO ACCOUNT FOR RECURSIVE SYNCING WITH NON-EXISTENT NESTED DIRECTORIES
	mod->destDirPath = strdup(destDirPath);
	CHECK_ALLOC(mod->destDirPath);
	mod->nextMod = NULL;
	return mod;
}

void freeModification(Modification *mod)
{
	if (mod == NULL) { return; }
	free(mod->filePath);
	free(mod->srcDirPath);
	free(mod->destDirPath);
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
	printf("MODIFICATION LIST (%d modification%s needed):\n\n", modList->numMods, modList->numMods == 1 ? "" : "s");
	printf("%-50s %-50s %-50s\n", "FILEPATH", "SRC DIR PATH", "DEST DIR PATH");
	Modification *mod = modList->head;
	while (mod != NULL) {
		printf("%-50s %-50s %-50s\n", mod->filePath, mod->srcDirPath, mod->destDirPath);
		mod = mod->nextMod;
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END ~~~~~~ OF ~~~~~~ MODIFICATIONLIST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
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
