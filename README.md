# get_next_line

A C function that reads one line at a time from a file descriptor, built as part of the [42 School](https://42.fr) curriculum.

## Overview

`get_next_line` returns the next newline-terminated line from a file descriptor on each successive call. When the file is exhausted it returns `NULL`. The function works on any valid fd — regular files, standard input, pipes — and retains unread bytes between calls using a `static` buffer.

## Prototype

```c
char *get_next_line(int fd);
```

- **Returns** a heap-allocated string containing the next line (newline included if present), or `NULL` on EOF or error.
- The caller is responsible for `free()`-ing the returned string.

## Bonus — Multiple File Descriptors

The bonus version replaces the single `static char *rest` with `static char *rest[10240]`, giving each file descriptor its own independent read buffer. This allows interleaved calls across up to 10 240 open fds without state corruption.

## File Structure

```
get_next_line/
├── get_next_line.h               # Header for the mandatory version
├── get_next_line.c               # Core logic — single fd
├── get_next_line_utils.c         # Helper functions (mandatory)
├── get_next_line_bonus.h         # Header for the bonus version
├── get_next_line_bonus.c         # Core logic — multiple fds
├── get_next_line_utils_bonus.c   # Helper functions (bonus)
├── main.c                        # Test driver — single file
└── main_bonus.c                  # Test driver — three files interleaved
```

## How It Works

Each call goes through three stages:

1. **`read_join`** — reads `BUFFER_SIZE` bytes at a time from `fd`, appending each chunk to the `rest` accumulator via `ft_strjoin`, until a `\n` is found or EOF is reached.
2. **`extract_line`** — slices everything up to and including the first `\n` out of `rest` and returns it as the line.
3. **`new_rest`** — saves whatever comes after the `\n` back into `rest` for the next call.

## Buffer Size

`BUFFER_SIZE` defaults to `42` and can be overridden at compile time:

```bash
cc -D BUFFER_SIZE=1024 get_next_line.c get_next_line_utils.c -o gnl
```

Larger values mean fewer `read()` syscalls; smaller values are useful for testing edge cases. A value ≤ 0 causes `get_next_line` to return `NULL` immediately.

## Compilation

**Mandatory (single fd):**
```bash
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c main.c -o gnl
./gnl
```

**Bonus (multiple fds):**
```bash
cc -Wall -Wextra -Werror get_next_line_bonus.c get_next_line_utils_bonus.c main_bonus.c -o gnl_bonus
./gnl_bonus
```

The test mains expect `test.txt` (mandatory) and `file1.txt`, `file2.txt`, `file3.txt` (bonus) to exist in the working directory.

## Usage Example

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int main(void)
{
    int   fd;
    char *line;

    fd = open("file.txt", O_RDONLY);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

## Implementation Notes

- `ft_strjoin` frees `s1` after joining, so the accumulator is always moved forward cleanly — no double-free, no leak on the read loop.
- A `NULL` `s1` is treated as an empty string, allowing the very first call (where `rest` is uninitialized) to work without a special case.
- On a `read()` error, `free_all` releases both the temporary buffer and the current `rest` before returning `NULL`, preventing leaks on error paths.
- The last line of a file (no trailing `\n`) is returned normally; the next call finds an empty `rest` and returns `NULL`.

## Author

**mel-hyna** — 42 School student
