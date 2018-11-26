/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */


#include "cluf.h"

void cluf_symlink(char *srcDir) {
  char destPath[PATH_MAX];
  snprintf(destPath,PATH_MAX,"%s%s",_cluf.destDir,srcDir+_cluf.srcLen);
  if(_cluf.debug>2) {
    fprintf(stderr,"making symlings %s -> %s\n",srcDir,destPath);
  }
  DIR *srcD=opendir(srcDir);
  if(!srcD) {
    perror("open source dir for symlink");
    exit(EXIT_FAILURE);
  }
  int destFd=open(destPath,O_RDONLY);
  if(destFd<=0) {
    perror("open dest dir for symlink");
    exit(EXIT_FAILURE);
  }
  if(cluf_same(dirfd(srcD),destFd))
  {
    if(_cluf.debug>2)
      fprintf(stderr,"same directories in symlink\n");
    close(destFd);
    closedir(srcD);
    return;
  }
  int srcLen=snprintf(destPath,PATH_MAX,"%s/",srcDir);
  struct dirent *dirent;
  while(dirent=readdir(srcD)) {
    if(!strcmp(dirent->d_name,".")||(!strcmp(dirent->d_name,"..")))
      continue;
    if(_cluf.debug>3)
      fprintf(stderr,"making symlink for %s\n",dirent->d_name);
    strcpy(destPath+srcLen,dirent->d_name);
    if(symlinkat(destPath,destFd,dirent->d_name)){
      perror(destPath);
    }
  }
  close(destFd);
  closedir(srcD);
}
