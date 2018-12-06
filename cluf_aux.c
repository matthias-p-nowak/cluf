/**
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "cluf.h"

bool cluf_same(int fd1, int fd2) {
  /**
   * compares the stat structures, return true if the files are the same
   */
  struct stat fd1stat,fd2stat;
  if(fstat(fd1,&fd1stat)||fstat(fd2,&fd2stat)) {
    cluf_exit("stat on fd1 and fd2");
  }
  return ((fd1stat.st_dev==fd2stat.st_dev)&&
          (fd1stat.st_ino==fd2stat.st_ino));
}
