#include <stdlib.h> /* rand */
#include "utils.h"

int utils_random_int(int min, int max)
{
    if (min >= max)
        return min;
    return min + rand() % (max - min + 1);
}