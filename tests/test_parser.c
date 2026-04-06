#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/parser.h"

void test_clear_node(void)
{
    Node node;

    /* Manually dirty the struct */
    strncpy(node.id, "something", MAX_ID_LENGTH - 1);
    node.text_count = 5;
    node.choice_count = 3;
    node.has_secret = 1;
    node.is_continue = 1;

    parser_clear_node(&node);

    assert(node.id[0] == '\0');
    assert(node.text_count == 0);
    assert(node.choice_count == 0);
    assert(node.has_secret == 0);
    assert(node.is_continue == 0);

    printf("PASS: test_clear_node\n");
}

/* ----------------------------------------------------------------
 * test_missing_file
 *
 * Loading a nonexistent file should return 0, not crash. */
void test_missing_file(void)
{
    Node node;
    int result = parser_load_node("story/nonexistent.node", &node);
    assert(result == 0);
    printf("PASS: test_missing_file\n");
}

/* ----------------------------------------------------------------
 * test_load_basic_node
 *
 * Load the fixture file and verify every field is correct.
 *
 * strstr(haystack, needle) returns a pointer to the first
 * occurrence of needle inside haystack, or NULL if not found.
 * We use it to check that key words appear in the prose
 * without asserting the exact full string. */
void test_load_basic_node(void)
{
    Node node;
    int result = parser_load_node("story/test_fixture.node", &node);

    assert(result == 1);

    /* ID */
    assert(strcmp(node.id, "test_scene") == 0);

    /* Text variants */
    assert(node.text_count == 2);
    assert(strstr(node.texts[0], "cold") != NULL);
    assert(strstr(node.texts[1], "remember") != NULL);

    /* Secret */
    assert(node.has_secret == 1);
    assert(strcmp(node.secret, "hidden_room") == 0);

    /* Choices */
    assert(node.choice_count == 2);
    assert(strcmp(node.choices[0].label, "Stand up") == 0);
    assert(strcmp(node.choices[0].destination, "chair_scene") == 0);
    assert(strcmp(node.choices[1].label, "Leave the room") == 0);
    assert(strcmp(node.choices[1].destination, "@hub") == 0);

    /* Not a continue node */
    assert(node.is_continue == 0);

    printf("PASS: test_load_basic_node\n");
}

/* ----------------------------------------------------------------
 * test_load_prologue_node
 *
 * Verify that CONTINUE block is parsed correctly. */
void test_load_prologue_node(void)
{
    Node node;
    int result = parser_load_node("story/test_prologue.node", &node);

    assert(result == 1);
    assert(strcmp(node.id, "test_prologue") == 0);
    assert(node.text_count == 1);
    assert(node.is_continue == 1);
    assert(node.choice_count == 0);
    assert(strstr(node.continue_prompt, "enter") != NULL);

    printf("PASS: test_load_prologue_node\n");
}

int main(void)
{
    printf("--- parser tests ---\n");
    test_clear_node();
    test_missing_file();
    test_load_basic_node();
    test_load_prologue_node();
    printf("All parser tests passed.\n\n");
    return 0;
}