/*
 * cluf.h
 *
 *  Created on: Nov 20, 2018
 *      Author: me
 */

#ifndef CLUF_H_
#define CLUF_H_


#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



int cluf_same(int fd1, int fd2);
void cluf_setup(char* srcDir, char* recFile);
void cluf_symlink(char *srcDir);
void handle_events();


struct cluf_global {
    int debug;
    int fanotifyFD;
    FILE *fanotifyFile;
    int srcLen;
    char *destDir;
};

extern struct cluf_global _cluf;

#endif /* CLUF_H_ */
