# Node File Format

Story nodes live in `story/`. Each node is a plain text file
with the extension `.node`.

---

## Structure

A node file contains one NODE declaration and any combination of
TEXT, CONTINUE, CHOICE, and SECRET blocks.

The order of blocks in the file does not matter to the parser,
but the convention below is recommended for readability.

---

## NODE declaration

Must be the first line of the file.
Declares the unique ID of this scene.
Node IDs use underscores, no spaces.

```
NODE opening_library
```

---

## TEXT block

A prose passage for this scene.
Multiple TEXT blocks form the variant pool.

```
TEXT
The door gives without resistance. Inside, the smell of cold paper.
END
```

```
TEXT
You have been here before. The smell knows you.
END
```

**Rules:**
- `TEXT` and `END` must be on their own lines
- Everything between them is prose, printed exactly as written
- At least one TEXT block is required per node
- Variants are selected by visit count:
  first visit → TEXT 1, second visit → TEXT 2, and so on
- The last variant repeats on all subsequent visits
- Maximum 8 TEXT blocks per node

---

## CONTINUE block

Used in prologue-type nodes where no player choice is needed.
The engine displays the prompt and waits for the player to press enter.
No number is required.

```
CONTINUE
Press enter to begin.
END
```

**Rules:**
- `CONTINUE` and `END` must be on their own lines
- A node with a CONTINUE block must not have a CHOICE block
- Only one CONTINUE block per node
- The prompt text is a single line

---

## CHOICE block

The options available to the player at this scene.
Each line inside is one choice, in the format:

```
label | destination_node_id
```

```
CHOICE
Go deeper        | library_deep
Return to street | @hub
END
```

**Rules:**
- `CHOICE` and `END` must be on their own lines
- At least one choice required if no CONTINUE block
- Maximum 9 choices (player input is 1–9)
- Each choice line must contain exactly one ` | ` separator
  (space, pipe, space — all three characters required)
- Destination must be a valid node ID or the reserved word `@hub`
- A node with a CHOICE block must not have a CONTINUE block

---

## SECRET line

A hidden destination triggered when the player types `0`.
The renderer never displays it. The input handler checks for it silently.

```
SECRET library_hidden
```

**Rules:**
- Single line, no END marker
- At most one SECRET per node
- If no SECRET exists and the player types `0`, input re-prompts
- Destination must be a valid node ID (cannot be `@hub`)
- SECRET can appear in any node that also has a CHOICE block

---

## Reserved destinations

| Destination | Meaning                                  |
| ----------- | ---------------------------------------- |
| `@hub`      | Return to the hub node (scene selection) |

All other destinations are node IDs that must match a NODE declaration
in another file exactly.

---

## Reserved words

These words must appear only as block markers, never inside prose:

`NODE` `TEXT` `CONTINUE` `CHOICE` `SECRET` `END`

---

## Complete example — prologue node

```
NODE prologue

TEXT
There are places you return to without meaning to.
Not because they call you back — they do not call.
Because something in you has not finished with them yet.
END

CONTINUE
Press enter to begin.
END
```

---

## Complete example — scene node, first interior room

```
NODE library_entrance

TEXT
The door gives without resistance. Inside, the smell of cold paper.
A chair faces the window. No one has moved it in some time.
END

TEXT
You have been here before. The smell knows you.
The chair is where you left it. Everything is where you left it.
END

SECRET library_hidden

CHOICE
Go deeper        | library_deep
Return to street | @hub
END
```

---

## Complete example — checkpoint node

```
NODE library_deep

TEXT
The shelves here reach the ceiling. A ladder on a rail
runs the length of the far wall. You would need it.
END

TEXT
You came back to this. The ladder. You are not sure what you
expected to find the second time.
END

CHOICE
Climb the ladder     | library_summit
Leave this room      | library_entrance
Return to the street | @hub
END
```

---

## Complete example — scene end node

```
NODE library_summit

TEXT
From here you can see the whole room below.
It is smaller than it seemed from the floor.
Most things are.
END

TEXT
You have been up here before. It is still smaller than it seemed.
You are still surprised. You wonder when you will stop being surprised.
END

CHOICE
Descend            | library_entrance
Leave the building | @hub
END
```

---

## Node struct reference (for parser implementation)

What the parser produces from each file:

```c
typedef struct {
    char   id[MAX_ID_LENGTH];                       /* NODE value */
    char   texts[MAX_TEXT_VARIANTS][MAX_TEXT_LENGTH]; /* TEXT blocks */
    int    text_count;                              /* how many TEXT blocks */
    Choice choices[MAX_CHOICES];                    /* CHOICE lines */
    int    choice_count;                            /* how many choices */
    char   secret[MAX_ID_LENGTH];                   /* SECRET destination */
    int    has_secret;                              /* 1 if SECRET exists */
    int    is_continue;                             /* 1 if CONTINUE block */
    char   continue_prompt[MAX_LABEL_LENGTH];       /* CONTINUE prompt text */
} Node;
```

---

## File naming convention

Node files are named after their NODE id, with the `.node` extension.

```
story/prologue.node
story/hub.node
story/library_entrance.node
story/library_deep.node
story/library_summit.node
story/library_hidden.node
story/secrets/library_hidden.node   ← alternative: secrets subfolder
```

Flat structure is preferred. Scene nodes are grouped visually
by their shared prefix: all `library_` nodes sort together.

---

## Tips

- Every node file begins with `NODE id`
- TEXT and END are on their own lines, no trailing spaces
- Choice lines use ` | ` with spaces on both sides of the pipe
- SECRET is a single line with no END
- CONTINUE and CHOICE do not coexist in the same node
- Node IDs in destinations match existing files exactly
- No reserved words appear inside prose