#include "Robot_fleet_manager.h"

int main(){
    int count=0, reply=0;
    char ans;
    robot *fleet = NULL;
    
    FILE *database = fopen("fleet_data.bin", "rb");
    if(database != NULL){
        fread(&count, sizeof(int), 1, database); //Reading the number of robots which is present at the start of the bin file (if present)

        fleet = (robot*)malloc(count * sizeof(robot)); //Allocating Memory

        if (count > 0) {
            if (fleet == NULL) {
                printf("FATAL HARDWARE ERROR: Insufficient RAM to load the saved fleet! (Possible Save file corruption)\n");
                printf("Booting with an empty fleet to prevent system crash...\n");
                count = 0; //Force count to 0 to protect the main menu logic
            }
            else {
                //It is safe to dump the hard drive into RAM
                fread(fleet, sizeof(robot), count, database);
                printf("System Rebooted. Successfully loaded %d robots from the database.\n", count);
            }
        }
        else {
            printf("Save file found, but the fleet was empty.\n");
        }
        fclose(database); //Closing the file since allocation of memoery into RAM is done
    }
    else{
        printf("\nNo save file found! Initializing a New fleet...\n");

        count = get_safe_int("How many robots would you like to register?\n");
        fleet = (robot*)malloc(count * sizeof(robot));
        
        for(int i=0; i < count; i++){
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

    }
    
    while(1){
        printf("What would you like to do?\n");
        printf("1. Print Fleet Report.\n");
        printf("2. Modify a robot within the fleet\n");
        printf("3. Sort the fleet of Robots with respect to their battery level (Low to High).\n");
        printf("4. Run Sector proximity hardware check.\n");
        printf("5. Initiate emergency protocol! (Optimize utilization of robots with low battery).\n");
        reply = get_safe_int("Enter the number of the function to utilize: ");

        switch (reply){
            case 1:
            fleet_report(fleet, count);
            break;

            case 2:
            fleet = modify_fleet(fleet, &count);
            break;

            case 3:
            deployement_sorter(fleet, count);
            break;

            case 4:
            find_closest_robots(fleet, count);
            break;

            case 5:
            emergency_protocol(fleet, count);
            break;

            default:
            printf("Invalid option! Please enter a valid integer to choose your desired function call.\n");
            break;
        }
        check:
        printf("Would you like to use another function? (y/n)");
        scanf(" %c",&ans);
        if(ans == 'y' || ans == 'Y'){
            continue;
        }
        else if(ans == 'n' || ans == 'N'){
            printf("See you later! \n");
            break;
        }
        else{
            printf("Answer with either 'Y' for Yes or 'N' for No\n");
            goto check;
        }
        
    }
    database = fopen("fleet_data.bin", "wb");
    if(database != NULL){
        fwrite(&count, sizeof(count), 1, database); //dumping count of robots first in order.

        if(count > 0 && fleet != NULL){
            fwrite(fleet, sizeof(robot), count, database);
        }
        fclose(database);
    }
    else{
        printf("CRITICAL ERROR: Failed to write to Hard drive!\n");
    }
    free(fleet);
    return 0;
}
