/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "cluf.h"


void handle_event(const struct fanotify_event_metadata *metadata) {
  char path1[PATH_MAX];
  char path2[PATH_MAX];
  struct stat statbuf;
  snprintf(path1, sizeof(path1),"/proc/self/fd/%d", metadata->fd);
  int len = readlink(path1, path2,sizeof(path1) - 1);
  if(len==-1) {
    cluf_exit("failed link for readlink");
  }
  path2[len]='\0';
  int ret=fstat(metadata->fd, &statbuf);
  if(ret<0) {
    cluf_exit("fstat failed");
  }
  if(_cluf.debug>4) {
    printf("mask: %llu %s %s%s\n",metadata->mask,
           path2, _cluf.destDir, path2+_cluf.srcLen);
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
      fprintf(_cluf.fanotifyFile,"%s\n",path2);
    cluf_copyFile(path2,metadata->fd);
  }

  if(S_ISDIR(statbuf.st_mode))
  {
    if(_cluf.debug>1) {
      printf("got a directory\n");
      // TODO: update symlinks in that directory
    }

  }
  close(metadata->fd);
}

void handle_events() {
  /**
   *
   */
  struct fanotify_event_metadata buf[200];
  struct fanotify_event_metadata *metadata;
  ssize_t len;
  pid_t my_pid;
  printf("start handling events\n");
  my_pid=getpid();
  for(;;) {
    if(_cluf.debug>6)
      printf("another round\n");
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
