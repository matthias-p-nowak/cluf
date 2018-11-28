/*
 * cluf_setup.c
 *
 *  Created on: Nov 21, 2018
 *      Author: me
 */

#include "cluf.h"

void cluf_setup_1() {
  if(_cluf.sourceName==NULL)
    cluf_exit("no source directory given");
  _cluf.sourceLen=strlen(_cluf.sourceName);
  while((_cluf.sourceLen>0)&& (_cluf.sourceName[_cluf.sourceLen-1]=='/'))
    _cluf.sourceName[--_cluf.sourceLen]='\0';
  
  if(_cluf.targetName){
    _cluf.targetLen=strlen(_cluf.targetName);
  while((_cluf.targetLen>0)&& (_cluf.targetName[_cluf.targetLen-1]=='/'))
    _cluf.targetName[--_cluf.targetLen]='\0';
  }
  else {
    if(_cluf.debug>1)
      fprintf(stderr,"no destination directory, just watching\n");
  }
  if(_cluf.shortenLinks){
    if(strstr(_cluf.sourceName,_cluf.targetName)==NULL){
      // the target is not part of the source
      cluf_exit("shortening of symlinks not possible due to setup");
    }
  }
}

void cluf_setup(char* recFile) {
  /**
   * carries out the setup with the given functions
   */

  // *************************
  if(_cluf.debug>2)
    printf("setting up watched filesystem\n");
  _cluf.fanotifyFD = fanotify_init(FAN_CLOEXEC | FAN_CLASS_PRE_CONTENT, O_RDONLY);
  if (_cluf.fanotifyFD == -1) {
    cluf_exit("fanotify_init");
  }
  if (fanotify_mark(_cluf.fanotifyFD,
                    FAN_MARK_ADD | FAN_MARK_MOUNT,
                    FAN_CLOSE | FAN_ACCESS | FAN_MODIFY | FAN_OPEN | FAN_ONDIR,
                    AT_FDCWD, _cluf.sourceName) == -1) 
    cluf_exit("fanotify_mark");
  // *************************
  if(_cluf.destDir) {
    cluf_makeSymlinks(_cluf.sourceName);
  }
  // *************************
  if(recFile) {
    if(_cluf.debug>2)
      fprintf(stderr,"opening recording file %s\n",recFile);
    _cluf.fanotifyFile=fopen(recFile,"w+");
  }
}
