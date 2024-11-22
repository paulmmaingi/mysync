#include "headers/read_dir.h"

// FUNCTION DEFINITIONS

bool isHidden(char *filename) { return filename[0] == '.'; }

Directory *readDirectory(char *dirPath, OptionList *optList)
{
	DIR *dirp = opendir(dirPath);
	if (dirp == NULL) {
		fprintf(stderr, "Error opening directory: %s\n", dirPath);
		return NULL;
	}

	Directory *dir = initDirectory(dirPath);
	if (dir == NULL) { return NULL; }

	struct dirent *entry;
	while ((entry = readdir(dirp)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { continue; }
		if (!getOption(optList, 'a') && isHidden(entry->d_name)) { continue; }

		char *filePath = calloc(strlen(dirPath) + strlen(entry->d_name) + 2, sizeof(char));
		if (filePath == NULL) {
			perror(__func__);
			freeDirectory(dir);
			closedir(dirp);
			return NULL;
		}
		sprintf(filePath, "%s/%s", dirPath, entry->d_name);

		struct stat fileStat;
		if (lstat(filePath, &fileStat) == -1) {
			perror(__func__);
			free(filePath);
			freeDirectory(dir);
			closedir(dirp);
			return NULL;
		}

		if (S_ISREG(fileStat.st_mode)) {
			Option *opt;
			if ((opt = getOption(optList, 'i'))) {
				bool match = false;
				for (int i = 0; i < opt->numArgs; i++) {
					char *re = glob2regex(opt->args[i]);
					if (re == NULL) {
						fprintf(stderr, "Error converting glob to regex for pattern: %s\n", opt->args[i]);
						free(filePath);
						freeDirectory(dir);
						closedir(dirp);
						return NULL;
					}
					regex_t regex;
					int reti = regcomp(&regex, re, REG_EXTENDED | REG_NOSUB);
					if (reti != 0) {
						fprintf(stderr, "Error compiling regex for pattern: %s\n", opt->args[i]);
						free(re);
						free(filePath);
						freeDirectory(dir);
						closedir(dirp);
						return NULL;
					}
					reti = regexec(&regex, entry->d_name, 0, NULL, 0);
					if (!reti) { match = true; }
					regfree(&regex);
					free(re);
					if (match) { break; }
				}
				if (match) {
					free(filePath);
					continue;
				}
			}

			if ((opt = getOption(optList, 'o'))) {
				bool match = false;
				for (int i = 0; i < opt->numArgs; i++) {
					char *re = glob2regex(opt->args[i]);
					if (re == NULL) {
						fprintf(stderr, "Error converting glob to regex for pattern: %s\n", opt->args[i]);
						free(filePath);
						freeDirectory(dir);
						closedir(dirp);
						return NULL;
					}
					regex_t regex;
					int reti = regcomp(&regex, re, REG_EXTENDED | REG_NOSUB);
					if (reti != 0) {
						fprintf(stderr, "Error compiling regex for pattern: %s\n", opt->args[i]);
						free(re);
						free(filePath);
						freeDirectory(dir);
						closedir(dirp);
						return NULL;
					}
					reti = regexec(&regex, entry->d_name, 0, NULL, 0);
					if (!reti) { match = true; }
					regfree(&regex);
					free(re);
					if (match) { break; }
				}
				if (!match) {
					free(filePath);
					continue;
				}
			}

			File *file = initFile(entry->d_name, filePath, fileStat.st_size, fileStat.st_mtime, fileStat.st_mode);
			if (file == NULL) {
				free(filePath);
				freeDirectory(dir);
				closedir(dirp);
				return NULL;
			}
			addFileToDirectory(dir, file);

		} else if (S_ISDIR(fileStat.st_mode)) {
			if (getOption(optList, 'r')) {
				Directory *subdir = readDirectory(filePath, optList);
				if (subdir == NULL) {
					free(filePath);
					freeDirectory(dir);
					closedir(dirp);
					return NULL;
				}
				addSubdirToDirectory(dir, subdir);
			}
		}
		free(filePath);
	}
	closedir(dirp);
	return dir;
}

DirectoryList *readDirectories(int argc, int optind, char *argv[], OptionList *optList)
{
	DirectoryList *dirList = initDirectoryList();
	if (dirList == NULL) { return NULL; }

	for (int i = optind; i < argc; i++) {
		Directory *dir = readDirectory(argv[i], optList);
		if (dir == NULL) {
			freeDirectoryList(dirList);
			return NULL;
		}
		addDirectoryToDirectoryList(dirList, dir);
	}
	return dirList;
}