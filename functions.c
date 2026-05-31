#include "functions.h"

/** Checks the validity of programs arguments
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return 0 if arguments are valid otherwise returns -1
 */
int args_validity(int argc, char **argv)
{
    // Checks if the number of arguments is correct
    if (argc != 6)
    {
        perror("Invalid number of arguments");
        return 1;
    }

    // Checks the number of lorries
    if (atoi(argv[1]) >= 10000 || atoi(argv[1]) < 0)
    {
        perror("Invalid number of lorries");
        return 1;
    }

    // Checks the number of cars
    if (atoi(argv[2]) >= 10000 || atoi(argv[2]) < 0)
    {
        perror("Invalid number of cars");
        return 1;
    }

    // Checks the ferry capacity
    if (atoi(argv[3]) > 100 || atoi(argv[3]) < 3)
    {
        perror("Invalid ferry capacity");
        return 1;
    }

    // Checks the vehicle arrival time
    if (atoi(argv[4]) > 10000 || atoi(argv[4]) < 0)
    {
        perror("Invalid vehicle arrival time");
        return 1;
    }

    // Checks the ferry time
    if (atoi(argv[5]) > 1000 || atoi(argv[5]) < 0)
    {
        fprintf(stderr, "Error: Invalid ferry time");
        return 1;
    }

    return 0;
}

/** Creates the ferry process, the process is killed, when all the vehicles are transported
 *
 * @param max_ride_time maximal time the ferry can take when sailing from one port to another
 * @param ferry_cap the amount of vehicles the ferry can transport (ferry capacity)
 * @param num_of_vehicles number of vehicles the ferry will be transporting
 * @return 1 on error, otherwise 1
 */
int create_ferry(int max_ride_time, int ferry_cap, int num_of_vehicles)
{
    // Ferry process creation
    pid_t ferry =  fork();
    if (ferry == -1)
    {
        perror("ferry fork failed");
        return 1;
    }
    // Child process
    if (ferry == 0) {
        // Runs the ferry
        ferry_logic(max_ride_time, ferry_cap, num_of_vehicles);

        // Killing the ferry process
        exit(0);
    }

    return 0;
}
// Function that takes the ferry from one port to another and ferries the vehicles
void ferry_logic(int max_ride_time, int ferry_cap, int num_of_vehicles)
{
    // Logging the ferry start
    log_and_print("P: started\n");


    // Pseudorandom initialisation
    // - XORed with getpid() to ensure randomness between processes
    srand(time(NULL));

    int wait_time = 0;
    // Ferry cars between ports
    while (*num_of_vehicles_transported < num_of_vehicles)
    {
        // Sailing to the port 0
        wait_time = rand() % max_ride_time + 1;
        usleep(wait_time);

        // Unloading and loading at port 0
        ferry_unload(0);
        ferry_load( ferry_cap, 0);

        // Were all the vehicles transported?
        if (*num_of_vehicles_transported == num_of_vehicles) break;

        // Sailing to the port 1
        wait_time = rand() % max_ride_time + 1;
        usleep(wait_time);

        // Unloading and loading at port 1
        ferry_unload(1);
        ferry_load(ferry_cap, 1);

    }
    // Sailing to dock
    wait_time = rand() % max_ride_time + 1;
    usleep(wait_time);

    // Logging the ferry start
    log_and_print("P: finish\n");

}

// Unloads the vehicles from the ferry
void ferry_unload(int port)
{
    log_and_print("P: arrived to %d\n", port);


    // Unloading all the vehicles
    while (*num_of_vehicles_on_ferry > 0)
    {
        sem_post(ferry_sem);
        sem_wait(boarding_leaving);
        (*num_of_vehicles_on_ferry)--;
        (*num_of_vehicles_transported)++;
    }
}

// Loads the vehicles on the ferry
void ferry_load(int ferry_cap, int port)
{
    int ferry_cap_taken = 0;
    // Logging the ferry start
    while (ferry_cap_taken < ferry_cap)
    {
        // Loads car and truck if possible
        load_vehicle_tuple(port, ferry_cap, &ferry_cap_taken);

        // fprintf(log_file,"\nferry_cap:%d taken:%d\n\n", ferry_cap, ferry_cap_taken);
        // fflush(log_file);

        // Is there any other car
        if (is_port_empty(port, ferry_cap_taken, ferry_cap))
            break;
    }

    // Logging the ferry start
    // fprintf(log_file,"P: leaving %d\n", port);
    // fflush(log_file);
    log_and_print("P: leaving %d\n", port);

}




// Loads a car and a truck or just a car or truck at a specified port
void load_vehicle_tuple(int port, int ferry_cap, int *ferry_cap_taken)
{
    if (port)
    {
        // Try to load a lorry
        load_vehicle(P1_lorry, P1_lorry_counter, ferry_cap_taken, 3, ferry_cap, num_of_vehicles_on_ferry);
        // Try to load a car
        load_vehicle(P1_car, P1_car_counter, ferry_cap_taken, 1, ferry_cap, num_of_vehicles_on_ferry);
    }
    else
    {
        // Try to load a lorry
        load_vehicle(P0_lorry, P0_lorry_counter, ferry_cap_taken, 3, ferry_cap, num_of_vehicles_on_ferry);
        // Try to load a car
        load_vehicle(P0_car, P0_car_counter, ferry_cap_taken, 1, ferry_cap, num_of_vehicles_on_ferry);
    }
}

// Loads a vehicle onto the ferry if possible
void load_vehicle(sem_t* vehicle_sem, int* vehicle_counter,
                 int* ferry_cap_taken, int capacity_cost,
                 int ferry_cap, int* num_of_vehicles_on_ferry)
{
    if (*vehicle_counter > 0 && *ferry_cap_taken + capacity_cost - 1 < ferry_cap)
    {
        sem_post(vehicle_sem);
        (*num_of_vehicles_on_ferry)++;
        *ferry_cap_taken += capacity_cost;
        (*vehicle_counter)--;
        sem_wait(boarding_leaving);
    }
}

// Checks whether the port is empty
int is_port_empty(int port, int ferry_cap_taken, int ferry_cap)
{
    if (port)
    {
        // Check for P1
        if ((*P1_lorry_counter == 0 && *P1_car_counter == 0)
            || (*P1_lorry_counter > 0 && *P1_car_counter == 0 && ferry_cap_taken + 3 > ferry_cap))
            return 1;
    } else
    {
        // Check for P0
        if ((*P0_lorry_counter == 0 && *P0_car_counter == 0)
            || (*P0_lorry_counter > 0 && *P0_car_counter == 0 && ferry_cap_taken + 3 > ferry_cap)
            )
            return 1;
    }
    return 0;
}

/** Creates the specified number of car and lorry processes
 * 
 * @param num_of_cars number of car processes to be started
 * @param num_of_lorries number of lorry processes to be started
 * @param arrival_time maximal time it takes for vehicle to arrive into the port
 * @return 1 on err otherwise 0
 */
int create_vehicles(int num_of_cars, int num_of_lorries, int arrival_time)
{
    // Creating a child car process for each car
    for (int i = 1; i <= num_of_cars; ++i)
    {
        if (fork_vehicle(i, arrival_time, CAR)) return 1;
    }

    // Creating a child lorry process for each lorry
    for (int i = 1; i <= num_of_lorries; ++i)
    {
        if (fork_vehicle(i, arrival_time, LORRY)) return 1;
    }
    return 0;
}

// Creates the vehicle subprocess
int fork_vehicle(int vehicle_id, int arrival_time, int vehicle_type)
{
    pid_t vehicle =  fork();
    if (vehicle == -1)
    {
        perror("car fork failed");
        return 1;
    }
    // Child process
    if (vehicle == 0)
    {
        // Picking the char for the log file
        char vehicle_char = ' ';
        vehicle_char = (vehicle_type == CAR) ? 'O' : 'N';

        // Giving it an id
        int id = vehicle_id;

        // Pseudorandom initialisation
        // - XORed with getpid() to ensure randomness between processes
        srand(getpid() ^ time(NULL));

        // Picking a starting port
        int port = rand() % 2;

        log_and_print("%c %d: started\n", vehicle_char, id);

        // Driving to the port
        int wait_time = rand() % arrival_time  + 1;
        usleep(wait_time);

        log_and_print("%c %d: arrived to %d\n", vehicle_char, id, port);

        // Waits for the ferry and it's transported by it
        take_ferry(id, port, vehicle_type);

        // Kill the vehicle process
        exit(0);
    }

    return 0;
}

// Takes from vehicle from one port to the next
void take_ferry(int id, int port, int vehicle_type)
{
    // Waits in port, until ferry takes it
    if (port)
    {
        // Waiting to be loaded
        if (vehicle_type == CAR)
        {
            (*P1_car_counter)++;
            sem_wait(P1_car);
        } else // lorry
        {
            (*P1_lorry_counter)++;
            sem_wait(P1_lorry);
        }
    } else
    {
        // Waiting to be loaded
        if (vehicle_type == CAR)
        {
            (*P0_car_counter)++;
            sem_wait(P0_car);
        } else // lorry
        {
            (*P0_lorry_counter)++;
            sem_wait(P0_lorry);
        }
    }
    // Boards the ferry and its ferried
    ferry_vehicle(id, port, vehicle_type);
}

// Ferries the vehicle from one port to another
void ferry_vehicle(int id, int port, int vehicle_type)
{
    // Picking the char for the log file
    char vehicle_char = ' ';
    vehicle_char = (vehicle_type == CAR) ? 'O' : 'N';

    log_and_print("%c %d: boarding\n", vehicle_char, id);

    // Boarded ferry
    sem_post(boarding_leaving);

    // Wait for ferry
    sem_wait(ferry_sem);

    log_and_print("%c %d: leaving in %d\n", vehicle_char, id, (port + 1) % 2);

    // Left ferry
    sem_post(boarding_leaving);
}


/** Maps the memory for shared counters
 * 
 * @return 0 if successful otherwise 1
 */
int map_counters() {
    const char *shm_name = "/counters";
    const size_t shm_size = NUM_OF_COUNTERS * sizeof(int); //

    // Create or open shared memory
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    // Set the size of the shared memory
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate failed");
        return 1;
    }

    // Map the shared memory
    void *ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Assign the pointers mapped memory
    output_counter = (int *)ptr;
    P0_car_counter = output_counter + 1;
    P0_lorry_counter = P0_car_counter + 1;
    P1_car_counter = P0_lorry_counter + 1;
    P1_lorry_counter = P1_car_counter + 1;
    num_of_vehicles_on_ferry = P1_lorry_counter + 1;
    num_of_vehicles_transported = num_of_vehicles_on_ferry + 1;

    // Initialize counters
    *output_counter = 1;
    *P0_car_counter = 0;
    *P0_lorry_counter = 0;
    *P1_car_counter = 0;
    *P1_lorry_counter = 0;
    *num_of_vehicles_on_ferry = 0;
    *num_of_vehicles_transported = 0;

    // Initialize the counter semaphore
    counter_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem_init(counter_sem, 1, 1) == -1) {
        perror("sem_init counter_sem failed");
        munmap(ptr, shm_size);
        fclose(log_file);
        return 1;
    }

    return 0;
}

/** Variadic function that increments the msg counter and logs the specified message into a log file
 *
 * @note might not be entirely safe, va_list might be invalid in a child
 * @return 0 if successful, otherwise return 1
 */

void log_and_print(char *format, ...) {
    // Formats the message into a buffer with all arguments
    char* buffer = malloc(4096);
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 4096, format, args);
    va_end(args);

    sem_wait(counter_sem); // lock

    // Get counter value and increment it
    int my_counter = (*output_counter)++;

    // Writing the message into a log file
    fprintf(log_file, "%d: %s", my_counter, buffer);
    fflush(log_file);

    sem_post(counter_sem); // unlock

    free(buffer);
}

/** Creates semaphores for port 1 and port 2 and the ferry
 *
 * @return 0 if successful, otherwise 1
 */
int port_n_ferry_sem_init()
{
    // Allocating the memory for semaphores
    P0_car = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    P1_car = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    P0_lorry = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    P1_lorry = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    ferry_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    boarding_leaving = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialising the P0_car semaphore
    if (sem_init(P0_car, 1, 0) == -1) {
        perror("sem_init P0_car failed");
        return 1;
    }
    // Initialising the P1_car semaphore
    if (sem_init(P1_car, 1, 0) == -1) {
        perror("sem_init P1_car failed");
        return 1;
    }
    // Initialising the P0_lorry semaphore
    if (sem_init(P0_lorry, 1, 0) == -1) {
        perror("sem_init P0_lorry failed");
        return 1;
    }
    // Initialising the P1_lorry semaphore
    if (sem_init(P1_lorry, 1, 0) == -1) {
        perror("sem_init P1_lorry failed");
        return 1;
    }
    // Initialising the ferry semaphore
    if (sem_init(ferry_sem, 1, 0) == -1) {
        perror("sem_init ferry_sem failed");
        return 1;
    }
    // Initialising the boarding_leaving semaphore
    if (sem_init(boarding_leaving, 1, 0) == -1) {
        perror("sem_init boarding_leaving failed");
        return 1;
    }

    return 0;
}

/// Closes all the semaphores and unmaps the memory allocated for them
void all_sem_close()
{
    // Destroying the semaphores
    sem_destroy(counter_sem);
    sem_destroy(P0_car);
    sem_destroy(P1_car);
    sem_destroy(P0_lorry);
    sem_destroy(P1_lorry);
    sem_destroy(ferry_sem);
    sem_destroy(boarding_leaving);

    // Unmapping the memory
    munmap(counter_sem, sizeof(sem_t));
    munmap(P0_car, sizeof(sem_t));
    munmap(P1_car, sizeof(sem_t));
    munmap(P0_lorry, sizeof(sem_t));
    munmap(P1_lorry, sizeof(sem_t));
    munmap(ferry_sem, sizeof(sem_t));
    munmap(boarding_leaving, sizeof(sem_t));

}

/// Clears and opens in the log file in an append mode
int log_file_innit()
{
    // Cleaning the log file
    fclose(fopen(LOG_FILE, "w"));

    // Opens the log file in an append mode
    log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL)
    {
        perror("Log file could not be opened");
        return 1;
    }

    return 0;
}

/// Destroys all the semaphores, cleans the memory allocated for them and closes the log file
void free_resources()
{
    all_sem_close();
    fclose(log_file);

}



