/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 * 
 * contains functions for the setup of the environment, initializes the global structure
 */

#include "cluf.h"

void cluf_setup_1() {
  /**
   * dedicated function for setting up the names of the directories
   * some of the fields are already set
   */
    _cluf.targetLen=0;
    _cluf.sourceLen=0;
    _cluf.startTime=time(NULL);
  if(_cluf.sourceName==NULL)
    cluf_exit("no source directory given");
  _cluf.sourceLen=strlen(_cluf.sourceName);
  // removing trailing '/'
  while((_cluf.sourceLen>0)&& (_cluf.sourceName[_cluf.sourceLen-1]=='/'))
    _cluf.sourceName[--_cluf.sourceLen]='\0';
  // *****
  if(_cluf.targetName){
    _cluf.targetLen=strlen(_cluf.targetName);
  // removing trailing '/'
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
   * carries out the setup and establishes the notify listening
   */
  // *************************
  cluf_setup_1();
  // setting up fanotify
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
  // setting up the target directory with symlinks so we have something to get notified
  if(_cluf.targetName) {
    cluf_makeSymlinks(_cluf.sourceName);
  }
  // *************************
  if(recFile) {
    if(_cluf.debug>2)
      fprintf(stderr,"opening recording file %s\n",recFile);
    _cluf.fanotifyFile=fopen(recFile,"w+");
  }
}
