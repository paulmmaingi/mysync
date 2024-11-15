#include "headers/mysync.h"

struct option longOptions[] = {
		{"help", no_argument, NULL, 'h'},
		{"recursive", no_argument, NULL, 'r'},
		{"hidden", no_argument, NULL, 'a'},
		{"ignore", required_argument, NULL, 'i'},
		{"nothing", no_argument, NULL, 'n'},
		{"only", required_argument, NULL, 'o'},
		{"preserve", no_argument, NULL, 'p'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, 0}};

#define OPTIONS "hrai:no:pv"

void usage(char *progName)
{
	fprintf(stderr, "Usage: %s [options] <directory1> <directory2> ...\n", progName);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help\t\t\tPrint this help message\n");
	fprintf(stderr, "  -r, --recursive\t\tRecursively sync directories\n");
	fprintf(stderr, "  -a, --hidden\t\t\tInclude hidden files\n");
	fprintf(stderr, "  -i, --ignore <pattern>\tIgnore files matching pattern\n");
	fprintf(stderr, "  -n, --nothing\t\t\tDo nothing, just print what would be done\n");
	fprintf(stderr, "  -o, --only <pattern>\t\tOnly sync files matching pattern\n");
	fprintf(stderr, "  -p, --preserve\t\tPreserve file attributes\n");
	fprintf(stderr, "  -v, --verbose\t\t\tVerbose output\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	char *progName = argv[0];
	if (argc < 3) { usage(progName); }

	OptionList *optList = initOptionList();
	CHECK_ALLOC(optList);

	int opt;
	while ((opt = getopt_long(argc, argv, OPTIONS, longOptions, NULL)) != -1) {
		switch (opt) {
		case 'h':
			freeOptionList(optList);
			usage(progName);
			break;
		case 'r':
			addOptionToOptionList(optList, 'r', NULL);
			break;
		case 'a':
			addOptionToOptionList(optList, 'a', NULL);
			break;
		case 'i':
			addOptionToOptionList(optList, 'i', optarg);
			break;
		case 'n':
			addOptionToOptionList(optList, 'n', NULL);
			break;
		case 'o':
			addOptionToOptionList(optList, 'o', optarg);
			break;
		case 'p':
			addOptionToOptionList(optList, 'p', NULL);
			break;
		case 'v':
			addOptionToOptionList(optList, 'v', NULL);
			break;
		default:
			freeOptionList(optList);
			usage(progName);
			break;
		}
	}

	if (argc - optind < 2) {
		fprintf(stderr, "%s: missing (%d) directory arguments; expecting at least 2\n", progName, 2 - (argc - optind));
		freeOptionList(optList);
		usage(progName);
	}

	printOptionList(optList);

	DirectoryList *dirList = readDirectories(argc, optind, argv, optList);
	if (dirList == NULL) {
		freeOptionList(optList);
		exit(EXIT_FAILURE);
	}

	printDirectoryList(dirList);

	freeDirectoryList(dirList);

	freeOptionList(optList);

	return 0;
}
