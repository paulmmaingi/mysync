#include "headers/mysync.h"

struct option longOptions[] = {{"help", no_argument, NULL, 'h'},
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
	fprintf(stderr, "Usage: %s [options] <directory1> <directory2> ...\n",
	        progName);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help\t\t\tPrint this help message\n");
	fprintf(stderr, "  -r, --recursive\t\tRecursively sync directories\n");
	fprintf(stderr, "  -a, --hidden\t\t\tInclude hidden files\n");
	fprintf(stderr, "  -i, --ignore <pattern>\tIgnore files matching pattern\n");
	fprintf(stderr,
	        "  -n, --nothing\t\t\tDo nothing, just print what would be done\n");
	fprintf(stderr,
	        "  -o, --only <pattern>\t\tOnly sync files matching pattern\n");
	fprintf(stderr, "  -p, --preserve\t\tPreserve file attributes\n");
	fprintf(stderr, "  -v, --verbose\t\t\tVerbose output\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	char *progName = argv[0];
	if (argc < 3) {
		usage(progName);
	}

	return 0;
}
