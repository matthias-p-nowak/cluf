
/*
 * cluf.h
 *
 *  Created on: Nov 20, 2018
 *      Author: me
 */

#ifndef CLUF_H_
#define CLUF_H_

#define _GNU_SOURCE

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/fs.h> // for RENAME_EXCHANGE
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


int cluf_same(int fd1, int fd2);
void cluf_copyFile(char* dest, int fd);
void cluf_setup(char* recFile);
void cluf_setup_1();
void cluf_symlink(char *srcDir);
int cluf_source2target(char *in, char *out);
int cluf_target2source(char *in, char *out);
int cluf_source2shortened(char *in, char *out);
void handle_events();



struct cluf_global {
    int debug; // increased verbosity
    int fanotifyFD; // file descriptor for fanotify 
    FILE *fanotifyFile; // for recording opened files
    int srcLen; // length of sourcefile mount point
    char *destDir; // path to destination
    char *sourceName; // source name where the proper files are
    int sourceLen;
    char *targetName; // place to put symlinks and opened proper files
    int targetLen;
    bool shortenLinks; // indicate a use in a chrooted environment
};

extern struct cluf_global _cluf;

#endif /* CLUF_H_ */
