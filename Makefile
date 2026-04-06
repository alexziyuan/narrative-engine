CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

narrative: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: tests/test_state.c tests/test_input.c src/state.c src/input.c src/utils.c
	$(CC) $(CFLAGS) -o run_state_tests tests/test_state.c src/state.c src/utils.c
	$(CC) $(CFLAGS) -o run_input_tests tests/test_input.c src/input.c src/utils.c
	$(CC) $(CFLAGS) -o run_parser_tests tests/test_parser.c src/parser.c src/utils.c
	./run_state_tests
	./run_input_tests
	./run_parser_tests

clean:
	rm -f src/*.o narrative run_state_tests run_input_tests run_parser_tests
