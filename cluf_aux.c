/**
*/

#include "cluf.h"

int cluf_same(int fd1, int fd2) {
  /**
   * compares the stat structures 
   */
  struct stat fd1stat,fd2stat;
  if(fstat(fd1,&fd1stat)||fstat(fd2,&fd2stat)) {
    perror("stat on fd1 and fd2");
    exit(EXIT_FAILURE);
  }
  return ((fd1stat.st_dev==fd2stat.st_dev)&&
          (fd1stat.st_ino==fd2stat.st_ino));

}
