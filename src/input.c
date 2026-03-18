#include <stdio.h>
#include <stdlib.h> /* atoi */
#include "input.h"

#define INPUT_BUFFER_SIZE 64

int input_get_choice(int max_choices)
{
    char buffer[INPUT_BUFFER_SIZE];
    int choice = -1;

    while (choice < 1 || choice > max_choices)
    {
        printf("\n> ");

        /* fgets returns NULL on end-of-file or error */
        if (fgets(buffer, INPUT_BUFFER_SIZE, stdin) == NULL)
        {
            return 0;
        }

        choice = atoi(buffer);

        if (choice < 1 || choice > max_choices)
        {
            printf("Invalid choice. Please enter a number between 1 and %d.\n", max_choices);
        }
    }

    /* Convert to 0-based index */
    return choice - 1;
}