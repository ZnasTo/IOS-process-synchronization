
#ifndef PROJ2_H
#define PROJ2_H

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include "functions.h"

// The counters used to ferry the vehicles
sem_t *P0_car;
sem_t *P1_car;
sem_t *P0_lorry;
sem_t *P1_lorry;
sem_t *ferry_sem;
sem_t *counter_sem;
sem_t *boarding_leaving;

// The counters used to ferry the vehicles
int *output_counter;
int *P0_car_counter;
int *P0_lorry_counter;
int *P1_car_counter;
int *P1_lorry_counter;
int *num_of_vehicles_on_ferry;
int *num_of_vehicles_transported;

// The semaphores used to ferry the vehicles
FILE* log_file;

#endif //PROJ2_H
