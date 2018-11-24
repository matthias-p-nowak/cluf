/*
 * cluf.h
 *
 *  Created on: Nov 20, 2018
 *      Author: me
 */

#ifndef CLUF_H_
#define CLUF_H_

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fanotify.h>
#include <unistd.h>


void cluf_setup(char* srcDir, char* destDir, char* recFile);

void print_usage(char *progname);

struct cluf_global {
    int fanotifyFD;
    FILE *fanotifyFile;
};

extern struct cluf_global _cluf;

#endif /* CLUF_H_ */
