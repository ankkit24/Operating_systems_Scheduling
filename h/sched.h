#include <stdio.h>
#include <proc.h>
#define EXPDISTSCHED 0
#define LINUXSCHED 1

void setschedclass(int type);

int getnextproc(int random_priority);
