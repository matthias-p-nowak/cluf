/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 *
 * contains functions handling the fanotify events
 */

#include "cluf.h"


void handle_event(const struct fanotify_event_metadata *metadata) {
  /**
   * handles a single metadata entry synchronously
   */
  char procFileName[PATH_MAX];
  char openedFileName[PATH_MAX];
  /*
   * getting the filename from a fd: this is the solution in the fanotify example
   * readlink of the fd in /proc - it requires a snprintf - not the most effective solution
   */
  snprintf(procFileName, sizeof(procFileName),"/proc/self/fd/%d", metadata->fd);
  int len = readlink(procFileName, openedFileName,sizeof(procFileName) - 1);
  if(len==-1) {
    cluf_exit("failed link for readlink");
  }
  openedFileName[len]='\0'; // TODO - is it necessary?
  if(_cluf.debug>4) {
    // printing out the type of access
    char targetFN[PATH_MAX]; // for reporting
    int targetLen; // dummy
    cluf_source2target(openedFileName,targetFN, &targetLen);
    fprintf(stderr, "\n\n (%4.2f) mask: %llu %s %s: ",
            difftime(time(NULL),_cluf.startTime), metadata->mask,
            openedFileName, targetFN);
    if(metadata->mask & FAN_ACCESS)
      fprintf(stderr,"A ");
    if(metadata->mask & FAN_MODIFY)
      fprintf(stderr,"M ");
    if(metadata->mask & FAN_CLOSE_WRITE)
      fprintf(stderr,"CW ");
    if(metadata->mask & FAN_CLOSE_NOWRITE)
      fprintf(stderr,"CNW ");
    if(metadata->mask & FAN_OPEN)
      fprintf(stderr,"O ");
    if(metadata->mask & FAN_ONDIR)
      fprintf(stderr,"OD ");
    if(metadata->mask & FAN_EVENT_ON_CHILD)
      fprintf(stderr,"EOC ");
    fprintf(stderr,"\n-------------\n");
  }
  // *****************************************
  // ***** what kind of file do we have? *****
  // *****************************************
  struct stat statbuf;
  int ret=fstat(metadata->fd, &statbuf);
  if(ret<0) {
    cluf_exit("fstat failed");
  }
  if(S_ISDIR(statbuf.st_mode))
  {
    // directories are ignored, we are here because of an accessed symlink
    if(_cluf.debug>1) {
      fprintf(stderr, "got a directory %s\n",openedFileName);
    }
  }
  else {
    // it is not a directory
    if(metadata->mask & FAN_OPEN) {
      // this means action - we will copy opened files
      if(_cluf.fanotifyFile)
        fprintf(_cluf.fanotifyFile,"%s\n",openedFileName);
      // and action
      cluf_copyFile(openedFileName,metadata->fd);
    }
  }
  // fanotify opens a file descriptor and leave it up to us to close it
  close(metadata->fd);
}

void handle_events() {
  /**
   * the function is called from the main function, reading from the fanotifyFD until a signal is received
   * the signal is handled in main and calls exit
   */
  printf("start handling events\n");
  struct fanotify_event_metadata buf[200]; // can hold 200 items - number taken from fanotify example
  struct fanotify_event_metadata *metadata; // pointer to a single entry
  ssize_t len;
  // all opened files are reported, this program opens directories ...
  pid_t my_pid;
  my_pid=getpid();
  for(;;) {
    /*
     * the forever loop
     */
    if(_cluf.debug>6)
      fprintf(stderr,"another round\n");
    // big read
    len = read(_cluf.fanotifyFD, (void *) &buf, sizeof(buf));
    if (len == -1 && errno != EAGAIN) {
      cluf_exit("reading from fanotify fd");
    }
    if(len<=0) {
      cluf_exit("end of fanotify events");
    }
    if(_cluf.debug>5) {
      fprintf(stderr,"got %lu entries in this round\n",len/sizeof(struct fanotify_event_metadata));
    }
    /* Point to the first event in the bufferl test; and advance*/
    for (metadata = buf; FAN_EVENT_OK(metadata, len);
         metadata = FAN_EVENT_NEXT(metadata, len)) {
      // the inside loop, one for each metadata
      if (metadata->vers != FANOTIFY_METADATA_VERSION) {
        cluf_exit("Mismatch of fanotify metadata version.");
      }
      if(metadata->fd ==FAN_NOFD) {
        cluf_exit("fanotify queue overflow");
      }
      if(metadata->pid==my_pid) {
        // ignore - it's me
      }
      else
        handle_event(metadata);
      // must close the metadata->fd
      close(metadata->fd);
    }
  }
}
