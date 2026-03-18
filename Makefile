CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

narrative: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: tests/test_state.c tests/test_input.c src/state.c src/input.c src/utils.c
	$(CC) $(CFLAGS) -o run_tests $^
	./run_tests

clean:
	rm -f src/*.o narrative run_tests
