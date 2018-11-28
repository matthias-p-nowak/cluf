/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */


#include "cluf.h"

void cluf_makeSymlinks(char *sourceDirName) {
  char targetPath[PATH_MAX]; // the target as seen by this program
  cluf_source2target(sourceDirName,targetPath);
  if(_cluf.debug>2) {
    fprintf(stderr,"making symlings %s -> %s\n",sourceDirName,targetPath);
  }
  DIR *sourceDir=opendir(sourceDirName);
  if(!sourceDir)
    cluf_exit("open source dir for symlink");
  int target_fd=open(targetPath,O_RDONLY);
  if(target_fd<=0)
    cluf_exit("open dest dir for symlink");
  if(cluf_same(dirfd(sourceDir),target_fd))
  {
    if(_cluf.debug>2)
      fprintf(stderr,"same directories in symlink\n");
    close(target_fd);
    closedir(sourceDir);
    return;
  }
  // int srcLen=snprintf(targetPath,PATH_MAX,"%s/",sourceDirName);
  struct dirent *dirent;
  while(dirent=readdir(sourceDir)) {
    if(!strcmp(dirent->d_name,".")||(!strcmp(dirent->d_name,"..")))
      continue;
    if(_cluf.debug>3)
      fprintf(stderr,"making symlink for %s\n",dirent->d_name);
    if(cluf_source2shortened2(sourceDirName,dirent->d_name,targetPath))
    if(symlinkat(targetPath,target_fd,dirent->d_name)) {
      perror(targetPath);
    }
  }
  close(target_fd);
  closedir(sourceDir);
}

