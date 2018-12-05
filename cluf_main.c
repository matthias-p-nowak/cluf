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
         "	-p <pid file>       record pid into that file\n"
         "	-t                  silly test option\n\n"
         "\n", progname, progname);
  exit(EXIT_SUCCESS);
}

void handle_signal(int signal) {
  /**
   * actually, pulling out the hard way, let the OS do the clean-up
   */
  fprintf(stderr, "shutting down cluf\n");
  exit(EXIT_SUCCESS);
}

void cluf_exit(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void record_pid(char *pidFile) {
  fprintf(stderr,"pid=%d\n",getpid());
  FILE *f=fopen(pidFile,"w");
  if(!f)
    cluf_exit("file couldn't be opened");
  fprintf(f,"%d",getpid());
  fclose(f);
}

void * startUpdatingSymlinks(void *dummy){
  if(_cluf.debug>3){
    fprintf(stderr,"start updating symlinks\n");
  }
  cluf_updateSymlinks(_cluf.targetName);
  return NULL;
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
    while ((opt = getopt(argc, argv, "dtvp:f:")) != -1) {
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
      case 'p':
        record_pid(optarg);
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
  if(_cluf.targetName){
    if(_cluf.debug>2){
      const size_t myStackSize=16*1024*1024; // 16 MByte
      fprintf(stderr,"starting symlink updating thread\n");
      pthread_t bgUpdateSymlink;
       pthread_attr_t attr;
       if(pthread_attr_init(&attr)){
         cluf_exit("thread initialization");
       }
      size_t stacksize;
      if(pthread_attr_getstacksize(&attr,&stacksize)){
        cluf_exit("getting stacksize failed");
      }
      if(stacksize < myStackSize){
        if(_cluf.debug>3)
          fprintf(stderr," increasing stack from %ld to %ld\n",stacksize,myStackSize);
          if(pthread_attr_setstacksize(&attr,myStackSize))
            cluf_exit("setting stack size");
      }
      if(pthread_create(&bgUpdateSymlink,&attr,startUpdatingSymlinks,NULL))
        cluf_exit("creating background symlink thread");
    }
  }
  cluf_handle_events();
  // ***** should not arrive here *****
  // the signal handler has an exit
  if(_cluf.debug>0)
    printf("all done\n");
  exit(EXIT_SUCCESS);
}
