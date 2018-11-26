/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "cluf.h"


void handle_event(const struct fanotify_event_metadata *metadata){
  char path1[PATH_MAX];
  char path2[PATH_MAX];
  struct stat statbuf;
  snprintf(path1, sizeof(path1),"/proc/self/fd/%d", metadata->fd);
  int len = readlink(path1, path2,sizeof(path1) - 1);
  if(len==-1){
    fprintf(stderr,"failed link for readlink\n");
    exit(EXIT_FAILURE);
  }
  path2[len]='\0';
  int ret=fstat(metadata->fd, &statbuf);
  if(ret<0){
    perror("fstat failed");
    exit(EXIT_FAILURE);
  }
  if(_cluf.debug>1){
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
    if(S_ISDIR(statbuf.st_mode))
    {
      if(_cluf.debug>1){
        printf("got a directory\n");
      }
    }
  close(metadata->fd);
}

void handle_events () {
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
    if(_cluf.debug>3)
      printf("another round\n");
    len = read(_cluf.fanotifyFD, (void *) &buf, sizeof(buf));
    if (len == -1 && errno != EAGAIN) {
      perror("reading from fanotify fd");
      exit(EXIT_FAILURE);
    }
    if(len<=0) {
      fprintf(stderr,"end of fanotify events");
      exit(EXIT_FAILURE);
    }
    /* Point to the first event in the buffer */    
    for (metadata = buf;FAN_EVENT_OK(metadata, len);
      metadata = FAN_EVENT_NEXT(metadata, len)) {
      if (metadata->vers != FANOTIFY_METADATA_VERSION) {
        fprintf(stderr,"Mismatch of fanotify metadata version.\n");
          exit(EXIT_FAILURE);
      }
      if(metadata->fd ==FAN_NOFD){
        perror("fanotify queue overflow");
        exit(EXIT_FAILURE);
      }
      if(metadata->pid==my_pid){
        close(metadata->fd);
      }
      else 
        handle_event(metadata);
        
    }
  }
}
