#ifndef STATE_H
#define STATE_H

/* ----------------------------------------------------------------
 * Constants — change later if needed.
 * ---------------------------------------------------------------- */
#define MAX_FLAGS 128
#define MAX_COUNTERS 32
#define MAX_NODES 256

/* ----------------------------------------------------------------
 * WorldState
 * The notepad. Everything true about the world at any moment.
 *
 * flags[]    — boolean facts. 0 = false, 1 = true.
 *              e.g. flags[0] = 1 means "player has met the voice"
 *
 * counters[] — numeric values that accumulate.
 *              e.g. counters[0] = 4 means "trust level is 4"
 *
 * visited[]  — how many times each node has been seen.
 *              visited[7] = 2 means node 7 has been visited twice.
 * ---------------------------------------------------------------- */
typedef struct
{
    int flags[MAX_FLAGS];
    int counters[MAX_COUNTERS];
    int visited[MAX_NODES];
} WorldState;

/* ----------------------------------------------------------------
 * Function declarations.
 * These tell other files "these functions exist."
 * The actual code is in state.c.
 * ---------------------------------------------------------------- */

/* Set everything to zero. Call once at the start of the game. */
void state_init(WorldState *ws);

/* Set flags[flag_id] to value (use 0 or 1). */
void state_set_flag(WorldState *ws, int flag_id, int value);

/* Return the value of flags[flag_id]. */
int state_get_flag(const WorldState *ws, int flag_id);

/* Add 1 to counters[counter_id]. */
void state_increment(WorldState *ws, int counter_id);

/* Record that node_id was visited. */
void state_mark_visited(WorldState *ws, int node_id);

/* Return how many times node_id has been visited. */
int state_get_visited(const WorldState *ws, int node_id);

#endif /* STATE_H */