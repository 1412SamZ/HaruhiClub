#pragma once
#include "myWheels.h"

#define STAT_PATH "/proc/stat"

typedef struct _CPU_PACKED
{
    char name[16];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned idle;
} CPU_OCCUPY;


void pcpu(CPU_OCCUPY* cpu){
    char buff[4096];
    FILE* fd = fopen(STAT_PATH, "r");
    if(fd == NULL)return;
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %u %u %u %u", cpu -> name, &cpu -> user, &cpu -> nice, & cpu -> system, &cpu -> idle);
    fclose(fd);

}








