#include "headers/sync.h"

// FUNCTION DEFINITIONS

void createIntermediateDirectories(ModificationList *modList, bool verbose)
{
	Modification *mod = modList->head;
	while (mod != NULL) {
		char *dirPath = strdup(mod->fileDestPath);
		CHECK_ALLOC(dirPath);
		char *slash = dirPath;

		while ((slash = strchr(slash, '/')) != NULL) {
			*slash = '\0';
			struct stat st;
			if (stat(dirPath, &st) == -1) {
				if (errno == ENOENT) {
					if (mkdir(dirPath, 0755) == -1) {
						perror(__func__);
						free(dirPath);
						return;
					}
					if (verbose) { printf("Created directory: \033[1;34m%s\033[0m\n", dirPath); }
				} else {
					perror(__func__);
					free(dirPath);
					return;
				}
			} else if (!S_ISDIR(st.st_mode)) {
				fprintf(stderr, "%s: %s is not a directory; A directory of the same name cannot be created\n", __func__, dirPath);
				free(dirPath);
				return;
			}
			*slash = '/';
			slash++;
		}
		free(dirPath);
		mod = mod->nextMod;
	}
}

void copyFile(char *srcPath, char *destPath, OptionList *optList)
{
	if (srcPath == NULL || destPath == NULL) { return; }

	int srcFile = open(srcPath, O_RDONLY);
	if (srcFile == -1) {
		fprintf(stderr, "Error opening file: %s\n", srcPath);
		return;
	}

	int destFile = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (destFile == -1) {
		fprintf(stderr, "Error opening file: %s\n", destPath);
		close(srcFile);
		return;
	}

	char *buffer = calloc(1, 1024);
	if (buffer == NULL) {
		perror(__func__);
		close(srcFile);
		close(destFile);
		return;
	}

	ssize_t bytesRead;
	while ((bytesRead = read(srcFile, buffer, sizeof(buffer))) > 0) {
		if (write(destFile, buffer, bytesRead) != bytesRead) {
			fprintf(stderr, "Error writing to file: %s\n", destPath);
			free(buffer);
			close(srcFile);
			close(destFile);
			return;
		}
	}
	if (bytesRead == -1) {
		fprintf(stderr, "Error reading from file: %s\n", srcPath);
		free(buffer);
		close(srcFile);
		close(destFile);
		return;
	}

	if (getOption(optList, 'p')) {
		struct stat st;
		if (stat(srcPath, &st) == -1) {
			fprintf(stderr, "Error getting file status: %s\n", srcPath);
			free(buffer);
			close(srcFile);
			close(destFile);
			return;
		}
		if (chmod(destPath, st.st_mode) == -1) {
			fprintf(stderr, "Error setting file permissions: %s\n", destPath);
			free(buffer);
			close(srcFile);
			close(destFile);
			return;
		}

		struct timeval times[2];
		times[0].tv_sec = st.st_atime;
		times[0].tv_usec = 0;
		times[1].tv_sec = st.st_mtime;
		times[1].tv_usec = 0;
		if (utimes(destPath, times) == -1) {
			fprintf(stderr, "Error setting file times: %s\n", destPath);
			free(buffer);
			close(srcFile);
			close(destFile);
			return;
		}
	}
	free(buffer);
	close(srcFile);
	close(destFile);
	if (getOption(optList, 'v')) {
		printf("Copied file: \033[1;32m%s\033[0m to \033[1;32m%s\033[0m\n", srcPath, destPath);
	}
}

void syncFiles(ModificationList *modList, OptionList *optList)
{
	Modification *mod = modList->head;
	while (mod != NULL) {
		createIntermediateDirectories(modList, getOption(optList, 'v') != NULL);
		copyFile(mod->fileSrcPath, mod->fileDestPath, optList);
		mod = mod->nextMod;
	}
}