/**
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 *
 * contains the symlink related function
 */


#include "cluf.h"

void cluf_makeSymlinks(char *sourceDirName) {
  /*
   * looks into the source directory and makes shortened symlinks so that the (possibly chrooted) programs get the right files
   */
  char targetPath[PATH_MAX]; // the target as seen by this program
  int targetLen;
  cluf_source2target(sourceDirName,targetPath, &targetLen);
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
    // nothing to do
    if(_cluf.debug>2)
      fprintf(stderr,"same directories in symlink\n");
    close(target_fd);
    closedir(sourceDir);
    return;
  }
  if(_cluf.debug>3)
    fprintf(stderr,"making symlinks\n");
  struct dirent *dirent;
  while(dirent=readdir(sourceDir)) {
    // going through the entries
    if(!strcmp(dirent->d_name,".")||(!strcmp(dirent->d_name,"..")))
      continue;
    if(_cluf.debug>3)
      fprintf(stderr,"making symlink for %s\n",dirent->d_name);
    // ***** target may be shortened *****
    if(cluf_source2shortened2(sourceDirName,dirent->d_name,targetPath))
      cluf_exit("shortening failed in makeSymlinks");
    if(symlinkat(targetPath,target_fd,dirent->d_name)) {
      perror(targetPath);
    }
  }
  close(target_fd);
  closedir(sourceDir);
}

void cluf_updateSymlinks(char *target) {
  if(_cluf.debug>5)
    fprintf(stderr, "updating symlinks in %s\n",target);
  DIR *sourceDir=opendir(target);
  if(!sourceDir)
    cluf_exit("open source dir for symlink");
  //
  int sourceDirFd=dirfd(sourceDir);
  struct dirent *dirent;
  while(dirent=readdir(sourceDir)) {
    // going through the entries
    if(!strcmp(dirent->d_name,".")||(!strcmp(dirent->d_name,"..")))
      continue;
    if(_cluf.debug>3)
      fprintf(stderr,"making symlink for %s\n",dirent->d_name);
  }
  //
  closedir(sourceDir);
}

