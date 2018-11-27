/**
 */
#include "cluf.h"

int cluf_source2target(char *in, char *out){
  if(!_cluf.targetName){
    fprintf(stderr,"programming error 2018-11-27/1\n");
    exit(EXIT_FAILURE);
  }
  // if(in[_cluf.sourceLen]=='/')
    snprintf(out,PATH_MAX,"%s%s",_cluf.targetName,in+_cluf.sourceLen);
  // else
  // snprintf(out,PATH_MAX,"%s/%s",_cluf.targetName,in+_cluf.sourceLen);
  if(_cluf.debug>15)
    printf("%s -> %s\n",in,out);
  return 0;
}
int cluf_target2source(char *in, char *out){
  return 0;
}
int cluf_source2shortened(char *in, char *out){
  return 0;
}
