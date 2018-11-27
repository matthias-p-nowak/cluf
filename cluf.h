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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/fs.h> // for RENAME_EXCHANGE


int cluf_same(int fd1, int fd2);
void cluf_copyFile(char* dest, int fd);
void cluf_setup(char* srcDir, char* recFile);
void cluf_symlink(char *srcDir);
void handle_events();


struct cluf_global {
    int debug; // increased verbosity
    int fanotifyFD; // file descriptor for fanotify 
    FILE *fanotifyFile; // for recording opened files
    int srcLen; // length of sourcefile mount point
    char *destDir; // path to destination
};

extern struct cluf_global _cluf;

#endif /* CLUF_H_ */
