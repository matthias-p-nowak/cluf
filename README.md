# cluf
Copy linked used  files

## Application case
For small systems with a limited disk space, one would like to have installed as few files as possible, only the files that are actually used. Other files could reside on an external harddisk or be accessible via NFS. This tool creates symlinks on the small disk pointing to the larger disk. The mount point of the larger disk is monitored via the *fanotify* system call. 

Files that are accessed are copied to the small disk, the symbolic link is replaced by the normal file. Directories are created and filled with symlinks as necessary. Symlinks in accessed directory are refreshed.

### Proposed use case
The program is included in a ramdisk. The root directory of the small disk is mounted. On a subdirectory on this small disk, the bigger disk is mounted. **Cluf** is started with the big disk path, the smaller disk path, an indicator to shorten links. When it goes into background (daemon mode), the script continues with a root_switch. Under normal use, all files are copied over. *Note*, directories for dev proc sys and similar have to be created manually.

## What to find in this project

### Mechanism for using precompiled header files
The makefile contains rules that create the gcc GCH files. Only one header file included in C-files can be a precompiled header file, since the following includes might have to respond to definitions in the precompiled file. This limitation is due to how compilers work.

### C unit test framework
This project uses CUNIT, which offers interactive/batch/output-xml ways to run the tests. In difference to unit-tests in other frameworks, there is no automatic discovery of tests, indicated by annotations or else. This project uses 2 Macros (CUNIT_SUITE + CUNIT_TEST) and several definitions of these macros to create definitions and register functions.