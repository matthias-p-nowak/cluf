/**
 */
#include "cluf.h"

int cluf_source2target(char *in, char *out) {
  if(!_cluf.targetName)
    cluf_exit("programming error 2018-11-27/1");
  // if(in[_cluf.sourceLen]=='/')
  snprintf(out,PATH_MAX,"%s%s",_cluf.targetName,in+_cluf.sourceLen);
  // else
  // snprintf(out,PATH_MAX,"%s/%s",_cluf.targetName,in+_cluf.sourceLen);
  if(_cluf.debug>15)
    printf("%s -> %s\n",in,out);
  return 0;
}

int cluf_target2source(char *in, char *out) {
  fprintf(stderr,"programming required 2018-11-28/1\n");
  exit(EXIT_FAILURE);
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

int cluf_source2shortened2(char *in, char *entry, char *out){
  int l=strlen(in);
  if(_cluf.shortenLinks) {
    if(l>_cluf.targetLen) {
      snprintf(out,PATH_MAX,"%s/%s",in+_cluf.targetLen,entry);
    }else{
      if(l<_cluf.targetLen){
        if(_cluf.debug>0)
          fprintf(stderr,"shortening isn't possible - '%s' is already too short\n",in);
        return 1;
      }
      // an entry in the root directory
      snprintf(out,PATH_MAX,"/%s",entry);
    }
  } else{
    snprintf(out,PATH_MAX,"%s/%s/%s",_cluf.targetName,in+_cluf.sourceLen,entry);
  }
  return 0;
}
