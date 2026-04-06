#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

/* ----------------------------------------------------------------
 * INTERNAL HELPERS
 * These are marked static — only visible inside parser.c.
 * No other module can call them. This is good practice:
 * expose only what other modules actually need.
 * ---------------------------------------------------------------- */

/* strip_newline
 *
 * fgets always keeps the \n at the end of the line it reads.
 * "Enter further\n" and "Enter further" are different strings.
 * This removes the \n (and \r on Windows) so comparisons work.
 *
 * strcspn(str, "\r\n") returns the index of the first \r or \n.
 * Setting that character to \0 ends the string there. */
static void strip_newline(char *str)
{
    str[strcspn(str, "\r\n")] = '\0';
}

/* trim_leading_spaces
 *
 * Returns a pointer to the first non-space character in str.
 * Does not modify the original string — just moves the pointer forward.
 * This lets writers indent their node files without breaking the parser. */
static const char *trim_leading_spaces(const char *str)
{
    while (*str == ' ' || *str == '\t')
    {
        str++;
    }
    return str;
}

/* ----------------------------------------------------------------
 * BLOCK STATE
 *
 * The parser reads the file one line at a time.
 * It needs to remember which block it is currently inside.
 *
 * An enum is a named set of integer constants.
 * BLOCK_NONE = 0, BLOCK_TEXT = 1, etc.
 * Using names instead of numbers makes the code readable.
 * ---------------------------------------------------------------- */
typedef enum
{
    BLOCK_NONE,
    BLOCK_TEXT,
    BLOCK_CHOICE,
    BLOCK_CONTINUE
} BlockState;

/* ----------------------------------------------------------------
 * parser_clear_node
 *
 * Zeroes every byte in the Node struct.
 * Always call this before loading into a Node for the first time,
 * or before reusing a Node variable for a second load.
 *
 * Without this, the struct contains whatever garbage happened
 * to be in that memory — which can cause very subtle bugs. */
void parser_clear_node(Node *node)
{
    memset(node, 0, sizeof(Node));
}

/* ----------------------------------------------------------------
 * parser_load_node
 *
 * Opens a .node file, reads it line by line, and fills in
 * a Node struct. Returns 1 on success, 0 on failure.
 *
 * The parser is a simple state machine:
 * it knows which block it is inside and handles lines accordingly.
 * ---------------------------------------------------------------- */
int parser_load_node(const char *filepath, Node *node)
{

    /* fopen opens a file for reading ("r" = read-only).
     * Returns NULL if the file doesn't exist or can't be opened.
     * Always check for NULL before using the FILE pointer. */
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        printf("parser error: cannot open file: %s\n", filepath);
        return 0;
    }

    parser_clear_node(node);

    char line[MAX_TEXT_LENGTH];
    BlockState block = BLOCK_NONE;

    /* fgets reads one line at a time.
     * It stops at \n or when it has read sizeof(line) - 1 characters.
     * It always adds \0 at the end.
     * It returns NULL on EOF or error. */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        strip_newline(line);
        const char *trimmed = trim_leading_spaces(line);

        /* Skip blank lines - allowed anywhere in the file */
        if (trimmed[0] == '\0')
            continue;

        /* ---- NODE declaration ----------------------------------------
         * strncmp compares the first N characters of two strings.
         * strncmp(trimmed, "NODE ", 5) == 0 means:
         * "the first 5 characters of trimmed are exactly 'NODE '"
         *
         * trimmed + 5 advances the pointer past "NODE ",
         * leaving just the ID value. */
        if (strncmp(trimmed, "NODE ", 5) == 0)
        {
            strncpy(node->id, trimmed + 5, MAX_ID_LENGTH - 1);
            block = BLOCK_NONE;
            continue;
        }

        /* ---- SECRET line ---------------------------------------------
         * Single line, no END marker.
         * strncmp(trimmed, "SECRET ", 7) checks for the keyword.
         * trimmed + 7 is the destination that follows it. */
        if (strncmp(trimmed, "SECRET ", 7) == 0)
        {
            strncpy(node->secret, trimmed + 7, MAX_ID_LENGTH - 1);
            node->has_secret = 1;
            block = BLOCK_NONE;
            continue;
        }

        /* ---- Block openers ------------------------------------------ */
        if (strcmp(trimmed, "TEXT") == 0)
        {
            if (node->text_count < MAX_TEXT_VARIANTS)
            {
                block = BLOCK_TEXT;
            }
            continue;
        }

        if (strcmp(trimmed, "CHOICE") == 0)
        {
            block = BLOCK_CHOICE;
            continue;
        }

        if (strcmp(trimmed, "CONTINUE") == 0)
        {
            node->is_continue = 1;
            block = BLOCK_CONTINUE;
            continue;
        }

        /* ---- END marker ---------------------------------------------
         * Closes whatever block is currently open.
         * If we were in a TEXT block, the variant is complete —
         * increment text_count to move to the next slot. */
        if (strcmp(trimmed, "END") == 0)
        {
            if (block == BLOCK_TEXT)
            {
                node->text_count++;
            }
            block = BLOCK_NONE;
            continue;
        }

        /* ---- Content lines ------------------------------------------
         * Any line that is not a keyword is content.
         * What we do with it depends on which block we are in. */
        if (block == BLOCK_TEXT)
        {
            /* Append this line to the current text variant.
             *
             * node->text_count is the index of the variant being built.
             * If the variant already has content, we add \n before
             * appending so multi-line prose keeps its line breaks.
             *
             * strncat appends safely: it never writes more than
             * the remaining space in the destination. */
            int idx = node->text_count;
            size_t current_len = strlen(node->texts[idx]);
            size_t remaining = MAX_TEXT_LENGTH - current_len - 1; // -1 for null terminator

            if (current_len > 0)
            {
                strncat(node->texts[idx], "\n", remaining);
                remaining--;
            }
            strncat(node->texts[idx], trimmed, remaining);
            continue;
        }

        if (block == BLOCK_CONTINUE)
        {
            /* The line inside CONTINUE is the prompt text.
             * Only one line expected. */
            strncpy(node->continue_prompt, trimmed, MAX_LABEL_LENGTH - 1);
            continue;
        }

        if (block == BLOCK_CHOICE)
        {
            /* Each choice line: "label | destination"
             *
             * strstr finds the first occurrence of " | " in the line.
             * If it returns NULL, the line is malformed — skip it
             * and warn the writer. */
            const char *separator = strstr(trimmed, " | ");
            if (separator == NULL)
            {
                printf("parser warning: malformed choice (missing ' | '): %s\n",
                       trimmed);
                continue;
            }

            int idx = node->choice_count;
            if (idx >= MAX_CHOICES)
            {
                printf("parser warning: too many choices in node %s\n",
                       node->id);
                continue;
            }

            /* Label: everything before " | "
             * separator - trimmed gives the number of characters
             * before the separator. */
            int label_len = (int)(separator - trimmed);
            if (label_len >= MAX_LABEL_LENGTH)
                label_len = MAX_LABEL_LENGTH - 1;
            strncpy(node->choices[idx].label, trimmed, label_len);
            node->choices[idx].label[label_len] = '\0';

            /* Destination: everything after " | " (skip 3 characters) */
            strncpy(node->choices[idx].destination,
                    separator + 3,
                    MAX_ID_LENGTH - 1);

            node->choice_count++;
            continue;
        }
    }

    fclose(fp);

    /* ---- Validation ------------------------------------------------
     * A node without an ID or without any text is unusable.
     * Return 0 so the caller knows something went wrong. */
    if (node->id[0] == '\0')
    {
        printf("parser error: no NODE declaration in %s\n", filepath);
        return 0;
    }

    if (node->text_count == 0)
    {
        printf("parser error: no TEXT block found in %s\n", filepath);
        return 0;
    }

    if (!node->is_continue && node->choice_count == 0)
    {
        printf("parser error: no CHOICE block and no CONTINUE in %s\n",
               filepath);
        return 0;
    }

    return 1;
}

/* ----------------------------------------------------------------
 * parser_print_node
 *
 * Prints everything the parser extracted from a file.
 * Use this whenever a test fails or prose looks wrong —
 * it shows you exactly what the parser saw, not what you intended.
 * ---------------------------------------------------------------- */
void parser_print_node(const Node *node)
{
    printf("=== Node: %s ===\n", node->id);
    printf("Text variants: %d\n", node->text_count);
    for (int i = 0; i < node->text_count; i++)
    {
        printf("  [%d]: %s\n", i, node->texts[i]);
    }
    if (node->is_continue)
    {
        printf("Continue prompt: %s\n", node->continue_prompt);
    }
    if (node->has_secret)
    {
        printf("Secret: %s\n", node->secret);
    }
    printf("Choices: %d\n", node->choice_count);
    for (int i = 0; i < node->choice_count; i++)
    {
        printf("  [%d]: \"%s\" -> %s\n",
               i,
               node->choices[i].label,
               node->choices[i].destination);
    }
    printf("===\n");
}
