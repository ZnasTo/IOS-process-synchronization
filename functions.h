
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/sem.h>
#include <pthread.h>
#include <string.h>

#define NUM_OF_COUNTERS 7
#define LOG_FILE "proj2.out"

// The semaphores used to ferry the vehicles (need to be initialised in the main process)
extern sem_t *P0_car;
extern sem_t *P1_car;
extern sem_t *P0_lorry;
extern sem_t *P1_lorry;
extern sem_t *ferry_sem;
extern sem_t *counter_sem;
extern sem_t *boarding_leaving;

// The counters used to ferry the vehicles (need to be initialised in the main process)
extern int *output_counter;
extern int *P0_car_counter;
extern int *P0_lorry_counter;
extern int *P1_car_counter;
extern int *P1_lorry_counter;
extern int *num_of_vehicles_on_ferry;
extern int *num_of_vehicles_transported;

// Log file into which the actions and vehicles are logged (needs to be opened [-a] in the main process )
extern FILE* log_file;

// Enum specifying the vehicle type
enum VEHICLE_TYPE
{
    CAR = 0,LORRY
};

// Function declarations
int args_validity(int argc, char **argv);
int create_ferry(int max_ride_time, int ferry_cap, int num_of_vehicles);
int create_vehicles(int num_of_cars, int num_of_lorries, int arrival_time);
int fork_vehicle(int car_id, int arrival_time, int vehicle_type);
void log_and_print(char *format, ...);
int map_counters();
void ferry_logic(int max_ride_time, int ferry_cap, int num_of_vehicles);
void ferry_unload(int port);
int port_n_ferry_sem_init();
void take_ferry(int id, int port, int vehicle_type);
void ferry_vehicle(int id, int port, int vehicle_type);
void all_sem_close();
void ferry_load(int ferry_cap, int port);
void load_vehicle_tuple(int port, int ferry_cap, int *ferry_cap_taken);
int is_port_empty(int port, int ferry_cap_taken, int ferry_cap);
int increment_counter();
void load_vehicle(sem_t* vehicle_sem, int* vehicle_counter,
                 int* ferry_cap_taken, int capacity_cost,
                 int ferry_cap, int* num_of_vehicles_on_ferry);
int log_file_innit();
void free_resources();

#endif //FUNCTIONS_H
