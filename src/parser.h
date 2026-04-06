#ifndef PARSER_H
#define PARSER_H

/* ----------------------------------------------------------------
 * Constants
 * ---------------------------------------------------------------- */
#define MAX_TEXT_VARIANTS 8
#define MAX_TEXT_LENGTH 2048
#define MAX_CHOICES 9
#define MAX_LABEL_LENGTH 64
#define MAX_ID_LENGTH 64

/* ----------------------------------------------------------------
 * Choice
 *
 * One displayed option. The player sees label, the engine
 * loads destination when the player picks it.
 * ---------------------------------------------------------------- */
typedef struct
{
    char label[MAX_LABEL_LENGTH];
    char destination[MAX_ID_LENGTH];
} Choice;

/* Node
 *
 * Everything the parser extracts from one .node file.
 *
 * id[]              — unique scene name, e.g. "library_entrance"
 * texts[][]         — pool of prose variants
 * text_count        — how many TEXT blocks were found
 * choices[]         — displayed options (1 through choice_count)
 * choice_count      — how many CHOICE lines were found
 * secret[]          — ghost destination, triggered by player typing 0
 * has_secret        — 1 if a SECRET line was found, 0 otherwise
 * is_continue       — 1 if this node has a CONTINUE block, not CHOICE
 * continue_prompt[] — the text inside the CONTINUE block
 * ---------------------------------------------------------------- */
typedef struct
{
    char id[MAX_ID_LENGTH];
    char texts[MAX_TEXT_VARIANTS][MAX_TEXT_LENGTH];
    int text_count;
    Choice choices[MAX_CHOICES];
    int choice_count;
    char secret[MAX_ID_LENGTH];
    int has_secret;
    int is_continue;
    char continue_prompt[MAX_TEXT_LENGTH];
} Node;

/* ----------------------------------------------------------------
 * Function declarations
 * ---------------------------------------------------------------- */

/* Read a .node file and populate a Node struct.
 * Returns 1 on success, 0 on failure. */
int parser_load_node(const char *filepath, Node *node);

/* Zero out a Node. Call before reusing a Node variable. */
void parser_clear_node(Node *node);

/* Print a Node to stdout. Use to debug output. */
void parser_print_node(const Node *node);

#endif /* PARSER_H */