#include "proj2.h"

int main(int argc, char *argv[])
{

    // Check the validity of arguments
    if (args_validity(argc, argv) == 1)
        return 1;

    // Clear and open the log file
    if (log_file_innit() == 1)
        return 1;

    // Initializing the semaphores
    if (port_n_ferry_sem_init() == 1)
        return 1;

    // Cleans all resources at exit
    atexit(free_resources);

    // Mapping the memory for counters
    if (map_counters() == 1)
        return 1;

    // Creating the ferry process
    if (create_ferry(atoi(argv[5]),atoi(argv[3]),
        atoi(argv[1]) + atoi(argv[2])) == 1)
        return 1;


    // Creating the processes for vehicles
    if (create_vehicles(atoi(argv[1]),
        atoi(argv[2]),
        atoi(argv[4])) == 1)
        return 1;

    return 0;
}