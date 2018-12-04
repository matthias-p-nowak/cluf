/**
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 * can be precompiled
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
#include <time.h>
#include <unistd.h>

/*
 * documentation to the function is in the corresponding files
 */

bool cluf_same(int fd1, int fd2);
void cluf_copyFile(char* dest, int fd);
void cluf_setup(char* recFile);
void cluf_setup_1();
void cluf_makeSymlinks(char *srcDir);
int cluf_source2target(char *in, char *out, int *len);
int cluf_target2source(char *in, char *out);
int cluf_source2shortened(char *in, char *out);
int cluf_source2shortened2(char *in, char *entry, char *out);
void handle_events();
void cluf_exit(char *msg);



struct cluf_global {
    int debug; // increased verbosity
    int fanotifyFD; // file descriptor for fanotify 
    FILE *fanotifyFile; // for recording opened files
    char *sourceName; // source name where the proper files are
    int sourceLen; // length of sourcefile mount point
    char *targetName; // place to put symlinks and opened proper files
    int targetLen;
    bool shortenLinks; // indicate a use in a chrooted environment
    time_t startTime;
};

extern struct cluf_global _cluf;

#endif /* CLUF_H_ */
