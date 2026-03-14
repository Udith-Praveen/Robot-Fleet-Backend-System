#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Robot_fleet_manager.h"

/*
Set of bits we are going to work with:
uint8_t is a 8 bit integer data type i.e. 0000 0001 (example 1)
Engine : Bit 0 (1 << 0)
Lights : Bit 1 (1 << 1)
Alarms : Bit 2 (1 << 2)
For Bits 3 to 7 : Unused (Can be used in the future)
*/
//HELPER FUNCTIONS
int copyname(char source[] , char dest[]){
    int i=0;
    while(source[i]!='\0' && source[i]!='\n'){
        dest[i]=source[i];
        i++;
    }
    dest[i]='\0';
    return 0;
}
int get_safe_int(const char *prompt) {
    int value;
    int status;
    while (1) {
        printf("%s", prompt);
        status = scanf("%d", &value);
        if (status == 1) {
            char next_char = getchar();
            if (next_char == '\n' || next_char == EOF) {
                return value;
            } else {
                ungetc(next_char, stdin);
            }
        }
        printf("Invalid input! Please enter a valid integer.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF); 
    }
}
int compare_battery(const void *a, const void *b) { //comparison helper function for qsort
    robot *robotA = (robot *)a; //Explicit Type casting of struct pointers
    robot *robotB = (robot *)b;
    //Subtract B's battery from A's battery for Low-to-High sorting
    return (robotA->battery - robotB->battery); //Returning after comparison result
}

//MAIN FUNCTIONS
void fleet_report(robot fleet[], int count){
    printf("---- Fleet Report ----\n");
    for(int i=0 ; i<count; i++){
        printf("ID : %d | Name: %s | Battery: %d%% | Status:  \n",fleet[i].id, fleet[i].name, fleet[i].battery);

        if( (fleet[i].hardware_status) & (1 << 0) ) { //Checking the status of the Engine of the robot
            printf(" [ENGINE]: ON\n");
        }
        else{
            printf(" [ENGINE]: OFF\n");
        }
        if( (fleet[i].hardware_status) & (1 << 1) ) { //Checking the status of the Lights of the robot  
            printf(" [LIGHTS]: ON\n");
        }
        else{
            printf(" [LIGHTS]: OFF\n");
        }
        if( (fleet[i].hardware_status) & (1 << 2) ){ //Checking the status of the Alarm of the robot
            printf(" [ALARM]: ON\n");
        }
        else{
            printf(" [ALARM]: OFF\n");
        }
    }
}

void deployement_sorter(robot fleet[], int count){
    if(count < 2){
        printf("Not enough robots to sort!\n");
        return;
    }
    qsort(fleet, count, sizeof(robot), compare_battery); //Time complexity: O(NlogN)
    printf("Fleet sorted by battery levels (Low to High).\n");
}
robot* modify_fleet(robot *fleet, int *count){
    int i=0, a=0;
    int choice;
    printf("\n--- Fleet Management Menu ---\n");
    printf("1. Add more robots\n");
    printf("2. Delete robots\n");
    printf("3. Modify a robot's hardware status\n");
    choice = get_safe_int("Enter your choice: ");

    if(choice == 1){
        int num = get_safe_int("How many more robots would you like to add to the fleet?: ");
        if (num <= 0) {
        printf("Invalid amount. You must add at least 1 robot.\n");
        return fleet;
        }
        robot *info = (robot*)realloc(fleet, (*count +num) * sizeof(robot));

        if(info != NULL){
            fleet = info;

            for(i = *count; i < (*count + num); i++){
                printf("\nRegistering new Robots into the Fleet...\n");
                fleet[i].id = get_safe_int("Enter the ID of the robot\n");
                fleet[i].battery = get_safe_int("Enter the battery(%%) of the Robot\n");
                char Name[30];
                printf("Enter the Name of the Robot: ");
                fgets(Name,29,stdin);
                Name[strcspn(Name, "\n")] = 0;
                copyname(Name , fleet[i].name);
                do{
                fleet[i].position.x = get_safe_int("\nEnter the X coordinate position (Must be >= 0): ");
                } while (fleet[i].position.x < 0);
                do{
                fleet[i].position.y = get_safe_int("Enter the Y coordinate position (Must be >= 0): ");
                } while (fleet[i].position.y < 0);
                uint8_t status = 0;
                fleet[i].hardware_status = (uint8_t)get_safe_int("\nEnter the hardware status of the Robot:");
            }
            *count = (*count + num); //Updating the original count of the robots in main.
            printf("Successfully Expanded the Fleet!\n");
        }
        else{
            printf("CRITICAL ERROR: Memory allocation Failed!\n");
        }
    }
    else if(choice == 2){
        if(*count == 0){
            printf("The fleet is already empty!\n");
            return fleet;
        }
        else {
            int num_del = get_safe_int("How many robots would you like to delete?: ");

            if(num_del > *count || num_del <= 0){
                printf("Invalid amount! You only have %d robots.\n", *count);
            }
            else {
                //The Deletion Loop
                for(int k = 0; k < num_del; k++){
                    printf("\n--- Deletion %d of %d ---\n", k + 1, num_del);
                    fleet_report(fleet, *count); //Showing the user the current position of the robots in the fleet to pick indices.
                    
                    int del_idx = get_safe_int("Enter the index (1 to N) of the robot to delete: ");

                    if(del_idx >= 1 && del_idx <= *count) {
                        int robots_to_shift = *count - del_idx;
                        if (robots_to_shift > 0) {
                            memmove(&fleet[del_idx - 1], &fleet[del_idx], robots_to_shift * sizeof(robot)); //Moving memory from source to destination
                        }
                        //Updating the global count
                        *count = *count - 1; 
                        printf("Robots crushed in active memory!\n");
                        
                    } 
                    else {
                        printf("Invalid index! Skipping this deletion...\n");
                    }
                }
                //Reallocating memory for the new size of fleet 
                        if(*count > 0){
                            robot *info = (robot*)realloc(fleet, *count * sizeof(robot)); //O(1) space complexity outside loop
                            if(info != NULL) { 
                                fleet = info; 
                            }
                        }
                        else { 
                            //edge case
                            //If the user deletes the only existing robot in the fleet (count = 1)
                            free(fleet);
                            fleet = NULL;
                            printf("The fleet is now completely empty.\n");
                        }
                        printf("Robot deleted successfully.\n");
            }
        }
    }
    else if(choice == 3){
        check:
        i = get_safe_int("Which robot's status would you like to modify?\n");

        if(i <= *count && i>0){
            fleet_report(fleet, *count);

            printf("What would you like to do?: \n");
            printf("1. Toggle Engine Bit of the robot\n");
            printf("2. Toggle Lights Bit of the robot\n");
            printf("3. Toggle Alarms Bit of the robot\n");
            a = get_safe_int("Enter your desired choice: ");

            if(a == 1){ 
                fleet[i-1].hardware_status ^= (1 << 0);
                printf("Toggled Engine successfully!\n");
            }
            else if(a == 2){
                fleet[i-1].hardware_status ^= (1 << 1);
                printf("Toggled Lights successfully!\n");
            }
            else if(a == 3){
                fleet[i-1].hardware_status ^= (1 << 2);
                printf("Toggled Alarms successfully!\n");
            }
            printf("After Operation: \n");
            fleet_report(fleet, *count);
        }
        else{
            printf("Only %d robots are present at the moment in the fleet! Please enter accordingly! \n", *count);
            goto check;
        }
    }
    else{
        printf("Invalid Choice! \n");
    }
    return fleet;
}

void find_closest_robots(robot fleet[], int count){ //Spatial Hashing uses reduced CPU cycles compared to normal distance comparisons
    if(count < 2){
        printf("Not enough robots to compare! \n");
        return;
    }
    int max_x = fleet[0].position.x, max_y = fleet[0].position.y; //Initializing first coordinate positions

    for(int i=1; i < count; i++){
        if(fleet[i].position.x > max_x){
            max_x = fleet[i].position.x;
        }
        if(fleet[i].position.y > max_y){
            max_y = fleet[i].position.y;
        }
    } //To find the maximum X and Y position in order to build a grid for further space partitioning.
    //Calculating the grid dimensions
    int grid_width = (max_x / CELL_SIZE) + 1;
    int grid_height = (max_y / CELL_SIZE) + 1;
    int total_cells = grid_width * grid_height;

    //Allocate a flat array of buckets for computing.
    gridcell *grid = (gridcell*)calloc(total_cells, sizeof(gridcell)); //initialized values to 0
    if (grid == NULL) {
        printf("CRITICAL: Grid memory allocation failed!\n");
        return;
    }

    for(int i = 0; i < count ; i++){
        int cell_x = fleet[i].position.x / CELL_SIZE;
        int cell_y = fleet[i].position.y / CELL_SIZE;
        int flat_index = (cell_y * grid_width) + cell_x; //Calculating each robot's index within a cell

        if(grid[flat_index].count < MAX_PER_CELL){ //Check for overflow in a designed bucket
            grid[flat_index].robot_indices[ grid[flat_index].count ] = i;

            grid[flat_index].count++; //Incrementing number of robots in the bucket
        }
        else{
            printf("Too many robots in a bucket of the grid!\n");
            free(grid);
            return;
        }
    }
    //Checking Proximity
    int min_dist = 2147483647; //To find min distance, comparing with maximum possible 32 bit int.
    int bot1_id = -1, bot2_id = -1; //Flag

    //Amortized Analysis of Asymptotic Time complexity
    for(int c = 0; c < total_cells; c++){
        if(grid[c].count >= 2){  //Skips cells with 2 or less robots in the spatial grid saving cpu cycles
            for(int a = 0; a < grid[c].count; a++){
                for(int b = a + 1; b < grid[c].count; b++){ //Comparing within the cell
                    int idx1 = grid[c].robot_indices[a];
                    int idx2 = grid[c].robot_indices[b]; //Actual location translation from the cell

                    int dx = fleet[idx1].position.x - fleet[idx2].position.x;
                    int dy = fleet[idx1].position.y - fleet[idx2].position.y;
                    int dist_sq = (dx * dx) + (dy * dy); //O(1) complexity calculation
                
                    if(dist_sq < min_dist){
                        min_dist = dist_sq;
                        bot1_id = fleet[idx1].id;
                        bot2_id = fleet[idx2].id;
                    }
                }
            }
        }
    }
    free(grid);
    if(bot1_id != -1){
        printf("\n[PROXIMITY ALERT]: Closest robots are ID: %d and ID: %d (Squared Distance: %d)\n", bot1_id, bot2_id, min_dist);
    } else {
        printf("\n[SYSTEM]: Fleet is safely dispersed. No robots are sharing the same grid sector.\n");
    }
}
void emergency_protocol(robot fleet[], int count){
    int i;

    for(i = 0; i < count; i++){
        if(fleet[i].battery < 15){

            fleet[i].hardware_status = (fleet[i].hardware_status & ~(1 << 0)) & ~(1 << 1) | (1 << 2); //Turns OFF Engine and Lights and Turns ON Alarms
        }
    }
    deployement_sorter(fleet, count);
}
