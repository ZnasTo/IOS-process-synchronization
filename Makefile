CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

.PHONY: all clean run zip


all: proj2 run

proj2: proj2.o functions.o
	$(CC) $^ $(CFLAGS) -lpthread -lrt -o $@

proj2.o: proj2.c proj2.h functions.h
	$(CC) $(CFLAGS) -c $< -o $@

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c $< -o $@


run:
	./proj2 5 5 10 10 10
clean:
	rm -f *.o proj2
zip:
	zip proj2.zip proj2.c proj2.h functions.c functions.h Makefile

