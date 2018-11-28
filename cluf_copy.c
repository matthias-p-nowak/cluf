/**
 */

#include "cluf.h"
#define BUFMAX 1024*1024  

void cluf_createDir(char *destPath,char *srcPath) {
  char tmpDir[PATH_MAX];
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
    cluf_exit("opening src dir for symlinks");
  }
  struct dirent *dirent;
  char linkPath[PATH_MAX];
  while(dirent=readdir(srcDir)) {
    if(!strcmp(dirent->d_name,".")||(!strcmp(dirent->d_name,"..")))
      continue;
    if(_cluf.debug>6)
      fprintf(stderr,"making symlink for %s\n",dirent->d_name);
    snprintf(linkPath,PATH_MAX,"%s/%s",srcPath,dirent->d_name);
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

void cluf_copyFile(char* source, int fd) {
  char destPath[PATH_MAX];
  char srcPath[PATH_MAX];
  int destLen=snprintf(destPath,PATH_MAX,"%s%s",_cluf.destDir,source+_cluf.srcLen);
  if(_cluf.debug>2)
    fprintf(stderr,"copying to %s\n",destPath);
  // have to check immediate directories, shouldn't be symbolic
  int corr=_cluf.srcLen-strlen(_cluf.destDir);
  for(int i=_cluf.srcLen+1; i<destLen; ++i) {
    if(destPath[i]=='/') {
      destPath[i]='\0';
      if(_cluf.debug>3)
        fprintf(stderr,"testing %s\n",destPath);
      struct stat sb;
      if(lstat(destPath,&sb)) {
        cluf_exit("testing directory for being a symbolic link");
      }
      if(S_ISLNK(sb.st_mode)) {
        strncpy(srcPath,source,i-corr);
        if(_cluf.debug>3)
          fprintf(stderr,"replace %s <- %s\n",destPath,srcPath);
        cluf_createDir(destPath,srcPath);
      }
      destPath[i]='/';
    }
  }
  struct stat sb;
  if(fstat(fd,&sb))
  {
    perror("can't stat the source file");
    exit(EXIT_FAILURE);
  }
  snprintf(srcPath,PATH_MAX,"%sXXXXXX",destPath);
  int fd2=mkstemp(srcPath);
  char buf[BUFMAX];
  ssize_t rr,rw;
  while(0< ( rr=read(fd,buf,BUFMAX))){
    rw=write(fd2,buf,rr);
    if(rw != rr){
      perror("file copying");
      exit(EXIT_FAILURE);
    }
  }
  close(fd2);
  if(chown(srcPath,sb.st_uid,sb.st_gid)){
    perror("changin owner");
    exit(EXIT_FAILURE);
  }
  if(chmod(srcPath,sb.st_mode)){
    perror("changing file mode");
    exit(EXIT_FAILURE);
  }
  if(rename(srcPath,destPath)){
    perror("error in final file symlink rename");
    exit(EXIT_FAILURE);
  }
}
