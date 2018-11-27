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
         "	-v                  increase verbosity\n"
         "	-t                  silly test option\n\n"
         "\n", progname, progname);
  exit(EXIT_SUCCESS);
}

void handle_signal(int signal){
  /**
   * 
   */
  fprintf(stderr, "shutting down cluf\n");
  exit(EXIT_SUCCESS);

}

int main(int argc, char **argv) {
    /**
     *
     */
    // initializing the global structure
    memset(&_cluf,0,sizeof(_cluf));
// *************************
    if (argc <= 1) {
        print_usage(argv[0]);
    }
    char *srcDir = NULL;
    int daemonMode = 0;
    char *recFile = NULL;
    {
        int opt;
        while ((opt = getopt(argc, argv, "dtvf:")) != -1) {
            switch (opt) {
            case 'd':
                daemonMode = 1;
                if(_cluf.debug>0)
                  fprintf(stderr,"setting debug mode\n");
                break;
            case 't':
                fprintf(stderr, "testing options\n");
                break;
            case 'v':
                ++_cluf.debug;
                fprintf(stderr,"setting verbose to %d\n",_cluf.debug);
                break;
            case 'f':
                recFile = optarg;
                if(_cluf.debug>0)
                  fprintf(stderr,"setting recording file to %s\n",recFile);
                break;
            default:
                print_usage(argv[0]);
            }
        }
        char * runArg[argc];
        for (int i = 0; i < argc; ++i)
            runArg[i] = NULL;
        for (int i = optind, j = 0; i < argc; ++i, ++j) {
            if(_cluf.debug>3)
                fprintf(stderr, "argv[%d]=%s\n", i, argv[i]);
            runArg[j] = argv[i];
        }
        if (argc > 1)
            _cluf.sourceName = runArg[0];
        if (argc > 2)
            _cluf.targetName = runArg[1];
    }
// *************************
// add signal handler
    signal(SIGTERM, handle_signal);
    signal(SIGQUIT,handle_signal);
// *************************
    cluf_setup(recFile);
// *************************
    if (daemonMode) {
        if (fork()) {
            if(_cluf.debug>0)
                fprintf(stderr, "daemon started\n");
            exit(EXIT_SUCCESS);
        }
    }
// *************************
    handle_events();
    if(_cluf.debug>0)
        printf("all done\n");
    exit(EXIT_SUCCESS);
}
