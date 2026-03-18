#include <stdio.h>
#include <assert.h>
#include "../src/state.h"

void test_init_zeros_everything(void)
{
    WorldState ws;
    state_init(&ws);

    assert(ws.flags[0] == 0);
    assert(ws.counters[0] == 0);
    assert(ws.visited[0] == 0);

    printf("PASS: test_init_zeros_everything\n");
}

void test_flag_set_and_get(void)
{
    WorldState ws;
    state_init(&ws);

    /* Set flag 3 to true */
    state_set_flag(&ws, 3, 1);
    assert(state_get_flag(&ws, 3) == 1);

    /* Set it back to false */
    state_set_flag(&ws, 3, 0);
    assert(state_get_flag(&ws, 3) == 0);

    /* Other flags should be unaffected */
    assert(state_get_flag(&ws, 4) == 0);

    printf("PASS: test_flag_set_and_get\n");
}

void test_counter_increment(void)
{
    WorldState ws;
    state_init(&ws);

    state_increment(&ws, 0);
    assert(ws.counters[0] == 1);

    state_increment(&ws, 0);
    assert(ws.counters[0] == 2);

    printf("PASS: test_counter_increment\n");
}

void test_visited_tracking(void)
{
    WorldState ws;
    state_init(&ws);

    assert(state_get_visited(&ws, 5) == 0);

    state_mark_visited(&ws, 5);
    assert(state_get_visited(&ws, 5) == 1);

    state_mark_visited(&ws, 5);
    assert(state_get_visited(&ws, 5) == 2);

    printf("PASS: test_visited_tracking\n");
}

void test_out_of_bounds_ignored(void)
{
    WorldState ws;
    state_init(&ws);

    /* These should do nothing silently, not crash */
    state_set_flag(&ws, -1, 1);
    state_set_flag(&ws, MAX_FLAGS, 1);
    state_increment(&ws, MAX_COUNTERS);
    state_mark_visited(&ws, MAX_NODES);

    printf("PASS: test_out_of_bounds_ignored\n");
}

int main(void)
{
    printf("--- state tests ---\n");
    test_init_zeros_everything();
    test_flag_set_and_get();
    test_counter_increment();
    test_visited_tracking();
    test_out_of_bounds_ignored();
    printf("All state tests passed.\n\n");
    return 0;
}