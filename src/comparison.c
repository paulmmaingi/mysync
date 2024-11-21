#include "headers/comparison.h"

// FUNCTION DEFINITIONS

File *compareFilesMtime(File *file1, File *file2)
{
	if (file1 == NULL || file2 == NULL) { return NULL; }
	if (file1->fileMtime == file2->fileMtime) { return NULL; }
	return file1->fileMtime > file2->fileMtime ? file1 : file2;
}

char *swapSrcDest(char *filePath, char *srcDirPath, char *destDirPath)
{
	if (filePath == NULL || srcDirPath == NULL || destDirPath == NULL) { return NULL; }
	char *newPath = calloc(strlen(filePath) + strlen(destDirPath) - strlen(srcDirPath) + 1, sizeof(char));
	if (newPath == NULL) { return NULL; }
	strcpy(newPath, destDirPath);
	strcat(newPath, filePath + strlen(srcDirPath));
	return newPath;
}

ModificationList *compareDirectories(Directory *dir1, Directory *dir2)
{
	if (dir1 == NULL || dir2 == NULL) { return NULL; }

	ModificationList *modList = initModificationList();
	if (modList == NULL) { return NULL; }

	// Compare files in dir1 with dir2
	File *file1 = dir1->files;
	while (file1 != NULL) {
		bool found = false;
		File *file2 = dir2->files;
		while (file2 != NULL) {
			if (strcmp(file1->fileName, file2->fileName) == 0) {
				found = true;
				// Compare mtimes and add newer file to modList
				File *newer = compareFilesMtime(file1, file2);
				if (newer != NULL) {
					char *destPath = swapSrcDest(
							newer->filePath,
							newer == file1 ? dir1->dirPath : dir2->dirPath,
							newer == file1 ? dir2->dirPath : dir1->dirPath);
					if (destPath == NULL) {
						freeModificationList(modList);
						return NULL;
					}
					Modification *mod = initModification(newer->filePath, newer == file1 ? dir1->dirPath : dir2->dirPath, destPath);
					free(destPath);
					if (mod == NULL) {
						freeModificationList(modList);
						return NULL;
					}
					addModificationToModificationList(modList, mod);
				}
				break;
			}
			file2 = file2->nextFile;
		}

		// file1 is not in dir2
		if (!found) {
			char *destPath = swapSrcDest(file1->filePath, dir1->dirPath, dir2->dirPath);
			if (destPath == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			Modification *mod = initModification(file1->filePath, dir1->dirPath, destPath);
			free(destPath);
			if (mod == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			addModificationToModificationList(modList, mod);
		}
		file1 = file1->nextFile;
	}

	// Check for files in dir2 that don't exist in dir1
	File *file2 = dir2->files;
	while (file2 != NULL) {
		bool found = false;
		file1 = dir1->files;
		while (file1 != NULL) {
			if (strcmp(file2->fileName, file1->fileName) == 0) {
				found = true;
				break;
			}
			file1 = file1->nextFile;
		}

		// file2 is not in dir1
		if (!found) {
			char *destPath = swapSrcDest(file2->filePath, dir2->dirPath, dir1->dirPath);
			if (destPath == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			Modification *mod = initModification(file2->filePath, dir2->dirPath, destPath);
			free(destPath);
			if (mod == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			addModificationToModificationList(modList, mod);
		}
		file2 = file2->nextFile;
	}
	return modList;
}

ModificationList *compareDirectoriesRecursive(Directory *dir1, Directory *dir2)
{
	if (dir1 == NULL || dir2 == NULL) { return NULL; }

	// Top-level comparison
	ModificationList *modList = compareDirectories(dir1, dir2);
	if (modList == NULL) { return NULL; }

	// Go through subdirectories, if subdir in one dir but not the other, compareDirectories between an empty directory and the subdir. If the subdir is in both, compareDirectories between the two subdirs
	Directory *subdir1 = dir1->headSubdir;
	while (subdir1 != NULL) {
		bool found = false;
		Directory *subdir2 = dir2->headSubdir;
		while (subdir2 != NULL) {
			if (strcmp(subdir1->dirName, subdir2->dirName) == 0) {
				found = true;
				ModificationList *subModList = compareDirectoriesRecursive(subdir1, subdir2);
				if (subModList == NULL) {
					freeModificationList(modList);
					return NULL;
				}
				// Combine modification lists
				Modification *mod = subModList->head;
				while (mod != NULL) {
					addModificationToModificationList(modList, mod);
					mod = mod->nextMod;
				}
				free(subModList);
			}
			subdir2 = subdir2->nextDir;
		}

		// subdir1 is not in dir2
		if (!found) {
			// Compare subdir1 with an empty directory which would be in dir2 so need swapSrcDest
			char *emptyDirPath = swapSrcDest(subdir1->dirPath, dir1->dirPath, dir2->dirPath);
			if (emptyDirPath == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			Directory *emptyDir = initDirectory(emptyDirPath);
			free(emptyDirPath);
			if (emptyDir == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			ModificationList *subModList = compareDirectoriesRecursive(emptyDir, subdir1);
			freeDirectory(emptyDir);
			if (subModList == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			// Combine modification lists
			Modification *mod = subModList->head;
			while (mod != NULL) {
				addModificationToModificationList(modList, mod);
				mod = mod->nextMod;
			}
			free(subModList);
		}
		subdir1 = subdir1->nextDir;
	}

	// Check for subdirs in dir2 that don't exist in dir1
	Directory *subdir2 = dir2->headSubdir;
	while (subdir2 != NULL) {
		bool found = false;
		subdir1 = dir1->headSubdir;
		while (subdir1 != NULL) {
			if (strcmp(subdir2->dirName, subdir1->dirName) == 0) {
				found = true;
				break;
			}
			subdir1 = subdir1->nextDir;
		}

		// subdir2 is not in dir1
		if (!found) {
			// Compare subdir2 with an empty directory which would be in dir1 so need swapSrcDest
			char *emptyDirPath = swapSrcDest(subdir2->dirPath, dir2->dirPath, dir1->dirPath);
			if (emptyDirPath == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			Directory *emptyDir = initDirectory(emptyDirPath);
			free(emptyDirPath);
			if (emptyDir == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			ModificationList *subModList = compareDirectoriesRecursive(emptyDir, subdir2);
			freeDirectory(emptyDir);
			if (subModList == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			// Combine modification lists
			Modification *mod = subModList->head;
			while (mod != NULL) {
				addModificationToModificationList(modList, mod);
				mod = mod->nextMod;
			}
			free(subModList);
		}
		subdir2 = subdir2->nextDir;
	}
	return modList;
}

ModificationList *compareAllDirectories(DirectoryList *dirList)
{
	if (dirList == NULL) { return NULL; }

	ModificationList *modList = initModificationList();
	if (modList == NULL) { return NULL; }

	Directory *dir1 = dirList->head;
	while (dir1 != NULL) {
		Directory *dir2 = dir1->nextDir;
		while (dir2 != NULL) {
			ModificationList *subModList = compareDirectoriesRecursive(dir1, dir2);
			if (subModList == NULL) {
				freeModificationList(modList);
				return NULL;
			}
			// Combine modification lists
			Modification *mod = subModList->head;
			while (mod != NULL) {
				addModificationToModificationList(modList, mod);
				mod = mod->nextMod;
			}
			free(subModList);
			dir2 = dir2->nextDir;
		}
		dir1 = dir1->nextDir;
	}
	return modList;
}