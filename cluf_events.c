/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "cluf.h"


void handle_event(const struct fanotify_event_metadata *metadata) {
  /**
   * handles a single metadata entry synchronously
   */
  char procFileName[PATH_MAX];
  char openedFileName[PATH_MAX];
  struct stat statbuf;
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
  int ret=fstat(metadata->fd, &statbuf);
  if(ret<0) {
    cluf_exit("fstat failed");
  }
  if(_cluf.debug>4) {
    char targetFN[PATH_MAX];
    int targetLen;
    cluf_source2target(openedFileName,targetFN, &targetLen);
    printf("mask: %llu %s %s%s\n",metadata->mask,
           openedFileName, _cluf.destDir, targetFN);
    if(metadata->mask & FAN_ACCESS)
      printf("A ");
    if(metadata->mask & FAN_MODIFY)
      printf("M ");
    if(metadata->mask & FAN_CLOSE_WRITE)
      printf("CW ");
    if(metadata->mask & FAN_CLOSE_NOWRITE)
      printf("CNW ");
    if(metadata->mask & FAN_OPEN)
      printf("O ");
    if(metadata->mask & FAN_ONDIR)
      printf("OD ");
    if(metadata->mask & FAN_EVENT_ON_CHILD)
      printf("EOC ");
    printf("\n");
  }
  if(metadata->mask & FAN_OPEN) {
    if(_cluf.fanotifyFile)
      fprintf(_cluf.fanotifyFile,"%s\n",openedFileName);
    cluf_copyFile(openedFileName,metadata->fd);
  }

  if(S_ISDIR(statbuf.st_mode))
  {
    if(_cluf.debug>1) {
      printf("got a directory %s\n",openedFileName);
      // TODO: update symlinks in that directory
    }
    cluf_makeSymlinks(openedFileName);
  }
  close(metadata->fd);
}

void handle_events() {
  /**
   * the function is called from the main function, reading from the fanotifyFD until a signal is received
   * the signal is handled in main and calls exit
   */
  printf("start handling events\n");
  struct fanotify_event_metadata buf[200];
  struct fanotify_event_metadata *metadata;
  ssize_t len;
  pid_t my_pid;
  my_pid=getpid();
  for(;;) {
    /*
     * the forever loop
     */
    if(_cluf.debug>6)
      printf("another round\n");
    // big read
    len = read(_cluf.fanotifyFD, (void *) &buf, sizeof(buf));
    if (len == -1 && errno != EAGAIN) {
      cluf_exit("reading from fanotify fd");
    }
    if(len<=0) {
      cluf_exit("end of fanotify events");
    }
    /* Point to the first event in the buffer */
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
