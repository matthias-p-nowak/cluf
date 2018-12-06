/**
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "cluf.h"
#define BUFMAX 1024*1024

void cluf_createDir(char *destPath,char *srcPath) {
  /**
   *
   */
  char tmpDir[PATH_MAX];
  // making a template for the temporary directory
  snprintf(tmpDir,PATH_MAX,"%sXXXXXX",destPath);
  if(!mkdtemp(tmpDir)) {
    // is also creates the directory!
    cluf_exit("creating temp dir template");
  }
  if(_cluf.debug>4)
    fprintf(stderr,"tmp dir: %s\n",tmpDir);
  // *************************
  int destFd=open(tmpDir,O_RDONLY);
  if(destFd<=0) {
    cluf_exit("opening temp dir for symlinks");
  }
  DIR *srcDir=opendir(srcPath);
  if(!srcDir) {
    snprintf(tmpDir,PATH_MAX,"opening src dir '%s' for symlinks",srcPath);
    cluf_exit(tmpDir);
  }
  struct dirent *dirent;
  char linkPath[PATH_MAX];
  while(dirent=readdir(srcDir)) {
    if(!strcmp(dirent->d_name,".")||(!strcmp(dirent->d_name,"..")))
      continue;
    if(_cluf.debug>6)
      fprintf(stderr,"making symlink for %s\n",dirent->d_name);
    // snprintf(linkPath,PATH_MAX,"%s/%s",srcPath,dirent->d_name);
    cluf_source2shortened2(srcPath,dirent->d_name,linkPath);
    if(symlinkat(linkPath,destFd,dirent->d_name)) {
      perror(linkPath);
    }
  }
  struct stat statBuf;
  if(fstat(dirfd(srcDir),&statBuf)) {
    cluf_exit("fstat on src dir");
  }
  if(chown(tmpDir,statBuf.st_uid,statBuf.st_gid)) {
    cluf_exit("chown on dest dir");
  }
  if(chmod(tmpDir,statBuf.st_mode)) {
    cluf_exit("chmod on dest dir");
  }
  closedir(srcDir);
  close(destFd);
  // *************************
  // rename refuses to work if old is directory and new one is symlink
  // glibc does not have a wrapper for the renameat2 call
  // using straight syscall
  if(syscall(SYS_renameat2,0,tmpDir,0,destPath,RENAME_EXCHANGE))
  {
    fprintf(stderr,"renaming %s -> %s\n",tmpDir,destPath);
    cluf_exit("renaming tmp dir to final");
  }
  // tmpDir now points to the old symlink to a directory
  if(unlink(tmpDir)) {
    cluf_exit("unlink symlink to dest dir");
  }
}

void cluf_copyFile(char* sourceFilePath, int fd) {
  /**
   * function copies the file that was used
   */
  char targetPath[PATH_MAX];
  char srcPath[PATH_MAX];
  // we need the path that is visible to this program, not the one for symlinks
  int destLen;
  cluf_source2target(sourceFilePath,targetPath,&destLen);
  if(_cluf.debug>2)
    fprintf(stderr,"copying to %s\n",targetPath);
  // have to check immediate directories, shouldn't be symbolic
  int corr=_cluf.sourceLen-_cluf.targetLen;
  if(_cluf.debug>7)
    fprintf(stderr,"checking for symlinked dirs in %s\n",targetPath+_cluf.targetLen);
  for(int i=_cluf.targetLen; i<destLen; ++i) {
    if(targetPath[i]=='/') {
      targetPath[i]='\0';
      if(_cluf.debug>5)
        fprintf(stderr,"testing %s\n",targetPath);
      struct stat sb;
      if(lstat(targetPath,&sb)) {
        cluf_exit("testing directory for being a symbolic link");
      }
      if(S_ISLNK(sb.st_mode)) {
        strncpy(srcPath,sourceFilePath,i+corr);
        srcPath[i+corr]='\0';
        if(_cluf.debug>3) {
          fprintf(stderr,"sfp=%s i=%d, corr=%d\n",sourceFilePath,i,corr);
          fprintf(stderr,"replace %s <- %s\n",targetPath,srcPath);
        }
        cluf_createDir(targetPath,srcPath);
      }
      targetPath[i]='/';
    }
  }
  struct stat sb;
  if(fstat(fd,&sb))
  {
    cluf_exit("can't stat the source file");
  }
  snprintf(srcPath,PATH_MAX,"%sXXXXXX",targetPath);
  int fd2=mkstemp(srcPath);
  if(_cluf.debug>3){
    fprintf(stderr,"the tmp file is %s\n",srcPath);
  }
  if(fd2 <= 0)
    cluf_exit("opening target tmp file went wrong");
  char buf[BUFMAX];
  ssize_t rr,rw;
  while(0< ( rr=read(fd,buf,BUFMAX))) {
    rw=write(fd2,buf,rr);
    if(rw != rr) {
      cluf_exit("file copying");
    }
  }
  close(fd2);
  if(chown(srcPath,sb.st_uid,sb.st_gid)) {
    cluf_exit("changin owner");
  }
  if(chmod(srcPath,sb.st_mode)) {
    cluf_exit("changing file mode");
  }
  if(rename(srcPath,targetPath)) {
    cluf_exit("error in final file symlink rename");
  }
}
