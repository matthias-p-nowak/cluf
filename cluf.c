/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "cluf.h"


struct cluf_global _cluf;


void print_usage(char *progname) {
  /**
   * @brief prints out the usage for the program
   */
	printf("Copy linked used files\n"

			"usage: %s [options] <watched directory>\n"
			"   or: %s [options] <watched directory> <target directory>\n\n"
			"Options:\n"
			"	-f <record file>    record all encountered accessed to that file\n"
			"	-d                  after initialization, start daemon mode\n"
			"	-t                  silly test option\n\n"
			"\n", progname, progname);
	exit(EXIT_FAILURE);
}

void handle_signal(int signal){
  /**
   * 
   */
  fprintf(stderr, "shutting down cluf");
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  /**
   * 
   */
  // initializing the global structure
  _cluf.fanotifyFile=NULL;
  _cluf.fanotifyFD=-1;
  
	if (argc <= 1) {
		print_usage(argv[0]);
	}
	char *srcDir = NULL;
	char *destDir = NULL;
	int daemonMode = 0;
	char *recFile = NULL;
	{
		int opt;
		while ((opt = getopt(argc, argv, "dtf:")) != -1) {
			switch (opt) {
			case 'd':
				daemonMode = 1;
				break;
			case 't':
				fprintf(stderr, "testing options\n");
				break;
			case 'f':
				recFile = optarg;
				break;
			default:
				print_usage(argv[0]);
			}
		}
		char * runArg[argc];
		for (int i = 0; i < argc; ++i)
			runArg[i] = NULL;
		for (int i = optind, j = 0; i < argc; ++i, ++j) {
			fprintf(stderr, "argv[%d]=%s\n", i, argv[i]);
			runArg[j] = argv[i];
		}
		if (argc > 1)
			srcDir = runArg[0];
		if (argc > 2)
			destDir = runArg[1];
	}
  
  // add signal handler
  signal(SIGTERM, handle_signal);
  signal(SIGQUIT,handle_signal);
  
	cluf_setup(srcDir, destDir, recFile);
	if (daemonMode) {
		if (fork()) {
			fprintf(stderr, "daemon started");
			exit(EXIT_SUCCESS);
		}
	}
	printf("all done\n");
	exit(EXIT_SUCCESS);
}
