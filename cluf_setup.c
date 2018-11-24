/*
 * cluf_setup.c
 *
 *  Created on: Nov 21, 2018
 *      Author: me
 */

#include "cluf.h"



void cluf_setup(char* srcDir, char* destDir, char* recFile) {
	if (srcDir == NULL) {
		fprintf(stderr, "no source directory given\n");
		exit(EXIT_FAILURE);
	}
    printf("setting up watched filesystem\n");
	_cluf.fanotifyFD = fanotify_init(FAN_CLOEXEC | FAN_CLASS_PRE_CONTENT, O_RDONLY);
	if (_cluf.fanotifyFD == -1) {
		perror("fanotify_init");
		exit(EXIT_FAILURE);
	}
	if (fanotify_mark(_cluf.fanotifyFD,
	FAN_MARK_ADD | FAN_MARK_MOUNT,
	FAN_CLOSE | FAN_ACCESS | FAN_MODIFY | FAN_OPEN | FAN_ONDIR,
	AT_FDCWD, srcDir) == -1) {
		perror("fanotify_mark");
		exit(EXIT_FAILURE);
	}
    if(recFile){
       _cluf.fanotifyFile=fopen(recFile,"w+");
    }
}
