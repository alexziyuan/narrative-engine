#ifndef INPUT_H
#define INPUT_H

/* ----------------------------------------------------------------
 * input_get_choice
 *
 * Shows a prompt, reads a line from the player, validates it.
 * Keeps asking until the player enters a valid number.
 *
 * max_choices: how many options are available (1 through max_choices, 10 for now)
 *
 * Returns: 0-based index of the chosen option.
 *          Player types "1" → returns 0
 *          Player types "2" → returns 1
 *          (0-based because C arrays start at 0)
 * ---------------------------------------------------------------- */
int input_get_choice(int max_choices);

#endif /* INPUT_H */