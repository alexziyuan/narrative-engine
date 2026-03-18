#include <stdio.h>
#include "../src/input.h"

/* ----------------------------------------------------------------
 * input_get_choice reads from stdin (the keyboard).
 * Automated testing requires redirecting stdin, which we'll
 * add later.
 *
 * For now: this file confirms the module compiles cleanly.
 * ---------------------------------------------------------------- */

int main(void)
{
    printf("input module: compiled successfully.\n");
    printf("\nManual test checklist (run ./narrative to verify):\n");
    printf("  [ ] Typing '1' with 2 choices returns first option\n");
    printf("  [ ] Typing '0' re-prompts\n");
    printf("  [ ] Typing 'abc' re-prompts\n");
    printf("  [ ] Typing '99' with 2 choices re-prompts\n");
    return 0;
}