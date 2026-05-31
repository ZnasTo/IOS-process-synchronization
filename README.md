Project: Ferry Simulator 
================================

Short description
-----------------
This is a small C project that simulates a ferry transporting cars and lorries between two ports using processes and POSIX semaphores. It demonstrates inter-process synchronization, shared memory counters, and logging.

Requirements
------------
- GCC with C99 support
- pthreads and realtime libraries (linker flags `-lpthread -lrt`)
- POSIX-compliant OS (Linux recommended)

Building
--------
Build using the provided `Makefile`:

```sh
make
```

This produces the executable `proj2`.

Running
-------
Run the program with five numeric arguments:

```sh
./proj2 <num_cars> <num_lorries> <ferry_capacity> <max_arrival_time> <max_ride_time>
```

Example (also available as `make run`):

```sh
./proj2 5 5 10 10 10
```

This will write an ordered log of events to `proj2.out`.

Files
-----
- `proj2.c` — program entrypoint, argument validation and orchestration.
- `proj2.h` — shared declarations of global variables used across files.
- `functions.c` — implementation of ferry and vehicle logic, shared memory and semaphore setup, logging helpers.
- `functions.h` — function prototypes and shared extern declarations.
- `Makefile` — build and convenience targets (`make`, `make run`, `make clean`, `make zip`).

Notes
-----
- The program uses POSIX shared memory and anonymous mmap for semaphores; run on Linux-like systems.
- The project expects valid numeric command-line arguments; invalid inputs will cause early exit (see argument checks in `functions.c`).
- Logs are appended to `proj2.out`; the program clears the file on start.

Suggested next steps
--------------------
- Add a simple test script that runs a few parameter combinations and validates `proj2.out` ordering.
- Add a `README` section describing the log format and expected output examples.
- Optionally add a small CI job to build and run basic smoke tests.

License
-------
This project is under GNU-GPL license