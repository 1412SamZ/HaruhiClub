#pragma once
#include "myWheels.h"

#define STAT_PATH "/proc/stat"

typedef struct 
{
    char name[16];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned idle;
} CPU_OCP;

typedef struct 
{
    char name[20];
    unsigned long total;
    char name2[20];
    unsigned long free;

}MEM_OCP;





void getCpuOccupy(CPU_OCP* cpu){
    char buff[4096];
    FILE* fd = fopen(STAT_PATH, "r");
    if(fd == NULL)return;
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %u %u %u %u", cpu -> name, &cpu -> user, &cpu -> nice, & cpu -> system, &cpu -> idle);
    fclose(fd);

}

double calCpuOccupy(CPU_OCP* cpu1, CPU_OCP* cpu2)
{
    double t1, t2;
    double id, sd;
    double cpuUsed;

    t1 = (double)(cpu1 -> user + cpu1->nice + cpu1 -> system + cpu1 -> idle);

    t2 = (double)(cpu2 -> user + cpu2->nice + cpu2 -> system + cpu2 -> idle);

    id = (double)(cpu2 -> user - cpu1 -> user);
    sd = (double)(cpu2 ->system - cpu1 -> system );

    cpuUsed = (100 * (id + sd)/(t2 - t1));

    return cpuUsed;

}






