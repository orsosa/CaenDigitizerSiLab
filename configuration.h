#ifndef _CONFIGURATION_file_
#define _CONFIGURATION_file_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


//extern int USB_HV;
//extern int USB_ard;
extern int model;
extern int polarization;
extern int vthreshold;
extern int acqsamples;
extern int nevents;
extern int bunches;
extern int ptriggersize;
extern int triggerpolaritymode;
//extern char filename[200];


extern char buf[200];
int ReadUntil(int fd, char tok);
int readconfig(char *file);

#endif
