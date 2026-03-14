#ifndef ROBOT_FLEET_MANAGER_H
#define ROBOT_FLEET_MANAGER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CELL_SIZE 50
#define MAX_PER_CELL 20 //safety limit, can be modified by the programmer.

typedef struct{
    int x;
    int y;

}coordinate;

typedef struct __attribute__((__packed__)){ //Removes padding of the compiler
    int id;
    char name[30];
    int battery;
    uint8_t hardware_status;
    coordinate position;

}robot;
//Tradeoff: This will ensue a micro-performance hit on the CPU since the alignment is done by the cpu itself and not by the compiler

typedef struct {
    int count;
    int robot_indices[MAX_PER_CELL];
}gridcell;

//HELPER FUNCTIONS
int copyname(char source[] , char dest[]);
int get_safe_int(const char *prompt);
int compare_battery(const void *a, const void *b);

//MAIN FUNCTIONS
void fleet_report(robot fleet[], int count);
void deployement_sorter(robot fleet[], int count);
robot* modify_fleet(robot *fleet, int *count);
void find_closest_robots(robot fleet[], int count);
void emergency_protocol(robot fleet[], int count);

#endif
