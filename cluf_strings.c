/**
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */
#include "cluf.h"

int cluf_source2target(char *in, char *out, int *len) {
  /**
   * put out the filepath that is related to the source path (in)
   * it means to transfer the remaining from one side to the other
   * no special cases need to be considered
   */
  if(!_cluf.targetName)
    cluf_exit("programming error 2018-11-27/1");
  if(_cluf.debug>6)
    fprintf(stderr,"source len: %d\n",_cluf.sourceLen);
  *len=snprintf(out,PATH_MAX,"%s%s",_cluf.targetName,in+_cluf.sourceLen);
  if(_cluf.debug>5)
    fprintf(stderr,"%s -> %s\n",in,out);
  return 0;
}

int cluf_source2shortened(char *in, char *out) {
  int l=strlen(in);
  if(_cluf.shortenLinks) {
    if(l>_cluf.targetLen) {
      // have something inside the target
      snprintf(out,PATH_MAX,"%s",in+_cluf.targetLen);
    }
    else {
      if(l<_cluf.targetLen) {
        if(_cluf.debug>0)
          fprintf(stderr,"shortening isn't possible - '%s' already is too short\n",in);
        return 1;
      }
      // having just the root
      strcpy(out,"/");
    }
  } else {
    snprintf(out,PATH_MAX,"%s%s",_cluf.targetName,in+_cluf.sourceLen);
  }
  return 0;
}

int cluf_source2shortened2(char *in, char *entry, char *out) {
  int l=strlen(in);
  if(_cluf.shortenLinks) {
    if(l>_cluf.targetLen) {
      if(_cluf.debug>5)
        fprintf(stderr,"shortening on the target\n");
      snprintf(out,PATH_MAX,"%s/%s",in+_cluf.targetLen,entry);
    } else {
      if(l<_cluf.targetLen) {
        if(_cluf.debug>0)
          fprintf(stderr,"shortening isn't possible - '%s' is already too short\n",in);
        return 1;
      }
      // an entry in the root directory
      if(_cluf.debug>5)
        fprintf(stderr,"having a root item in the shortened case : /%s\n",entry);
      snprintf(out,PATH_MAX,"/%s",entry);
    }
  } else {
      snprintf(out,PATH_MAX,"%s/%s",in,entry);
    }
  return 0;
}

int cluf_target2sourceShortened(char *in, char *entry, char *out){
  if(_cluf.shortenLinks){
    if(!strstr(in,_cluf.targetName)){
      snprintf(out,PATH_MAX,"asked source for a directory '%s' that isn't in the submitted target '%s'\n",in,_cluf.targetName);
      return 1;
    }
    // cut the common part
    if(strlen(in)==_cluf.targetLen){
      // the part after common from source+'/'+entry
      snprintf(out,PATH_MAX,"%s/%s",_cluf.sourceName+_cluf.targetLen,entry);
    }else{
      // have later parts from 'in' - the first slash is from the 'in' part
      snprintf(out,PATH_MAX,"%s%s/%s",_cluf.sourceName+_cluf.targetLen,in+_cluf.targetLen,entry);
    }
  }else{
    if(strlen(in)==_cluf.targetLen){
      // source dir and entry
      snprintf(out,PATH_MAX,"%s/%s",_cluf.sourceName,entry);
    }else{
      // source + later part of 'in'+entry. first '/' is from in
      snprintf(out,PATH_MAX,"%s%s/%s",_cluf.sourceName, in+_cluf.targetLen,entry);
    }
  }
  return 0;
}

int cluf_target2source(char *in, char *out){
  if(strlen(in)==_cluf.targetLen)
    strcpy(out,_cluf.sourceName); 
  else
    snprintf(out,PATH_MAX,"%s%s",_cluf.sourceName,in+_cluf.targetLen);
  return 0;
}
