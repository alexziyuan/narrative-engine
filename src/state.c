#include <string.h>
#include "state.h"

/* ----------------------------------------------------------------
 * state_init
 *
 * memset(pointer, value, size) fills a block of memory with a value.
 * We use it to zero out the entire WorldState in one call.
 *
 * sizeof(WorldState) returns the size in bytes of the struct.
 * We don't hardcode a number — if the struct changes, sizeof
 * automatically gives the right answer.
 * ---------------------------------------------------------------- */
void state_init(WorldState *ws)
{
    memset(ws, 0, sizeof(WorldState));
}

/* ----------------------------------------------------------------
 * The -> operator
 *
 * ws is a pointer to a WorldState, not a WorldState itself.
 * To access a field through a pointer, use -> instead of .
 *
 *   ws.flags[0]   — works if ws is a WorldState
 *   ws->flags[0]  — works if ws is a WorldState*  (pointer)
 *
 * They do the same thing. -> is shorthand for (*ws).flags[0].
 * ---------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * Bounds checking
 *
 * C does not stop you from writing ws->flags[9999].
 * It will silently overwrite unrelated memory and cause
 * mysterious bugs hours later.
 *
 * These checks return early if the index is out of range.
 * "return;" in a void function just exits immediately.
 * ---------------------------------------------------------------- */

void state_set_flag(WorldState *ws, int flag_id, int value)
{
    if (flag_id < 0 || flag_id >= MAX_FLAGS)
        return;
    ws->flags[flag_id] = value;
}

int state_get_flag(const WorldState *ws, int flag_id)
{
    if (flag_id < 0 || flag_id >= MAX_FLAGS)
        return 0;
    return ws->flags[flag_id];
}

void state_increment(WorldState *ws, int counter_id)
{
    if (counter_id < 0 || counter_id >= MAX_COUNTERS)
        return;
    ws->counters[counter_id]++;
}

void state_mark_visited(WorldState *ws, int node_id)
{
    if (node_id < 0 || node_id >= MAX_NODES)
        return;
    ws->visited[node_id]++;
}

int state_get_visited(const WorldState *ws, int node_id)
{
    if (node_id < 0 || node_id >= MAX_NODES)
        return 0;
    return ws->visited[node_id];
}