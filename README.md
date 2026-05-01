# 📄 get_next_line — 42 School Project

> **Author:** mel-hyna (`mel-hyna@student.42.fr`)
> **School:** 42 Network
> **Language:** C
> **Compiled with:** `cc -Wall -Wextra -Werror`
> **Key concept:** Static variables, file descriptors, buffered reading

---

## 📖 Overview

**get_next_line** is the second major project at 42 School. The goal is to implement a single function that reads one line at a time from a file descriptor — and returns it, including the trailing `\n` if there is one.

What makes this project challenging is that you must handle:
- Any `BUFFER_SIZE` (from 1 to millions), set at compile time
- Persistent state between calls (using a `static` variable)
- Memory leaks — every `malloc` must eventually be `free`d
- The **bonus**: reading from **multiple file descriptors simultaneously**, each with its own independent state

This function is used in virtually every future 42 project that reads from files or stdin.

---

## 📁 Project Structure

```
get_next_line-main/
├── get_next_line.h              ← Header for mandatory part
├── get_next_line.c              ← Main function (mandatory)
├── get_next_line_utils.c        ← Helper functions (mandatory)
├── get_next_line_bonus.h        ← Header for bonus part
├── get_next_line_bonus.c        ← Main function (bonus, multi-fd)
├── get_next_line_utils_bonus.c  ← Helper functions (bonus)
├── main.c                       ← Test: single file descriptor
└── main_bonus.c                 ← Test: multiple file descriptors
```

Total: **8 files** (split into mandatory and bonus versions)

---

## ⚙️ How to Compile

There is no Makefile in this project — you compile `get_next_line` directly with your own program.

### Mandatory (single fd)

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c your_main.c -o gnl
```

### Bonus (multiple fds)

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c your_main.c -o gnl_bonus
```

### Setting a custom `BUFFER_SIZE`

The `BUFFER_SIZE` macro controls how many bytes are read from the fd in each `read()` call. It defaults to `42` if not specified at compile time, but you can override it:

```bash
cc -D BUFFER_SIZE=1    ...   # Read byte by byte (slow but correct)
cc -D BUFFER_SIZE=4096 ...   # Standard page-size buffer (fast)
cc -D BUFFER_SIZE=1000000 .. # Very large buffer
```

Your implementation **must handle all of these correctly.**

---

## 🔍 Function Reference

### Main Function

```c
char *get_next_line(int fd);
```

| Parameter | Description                                       |
|-----------|---------------------------------------------------|
| `fd`      | File descriptor to read from (file, stdin, pipe, etc.) |

**Returns:**
- A heap-allocated string containing the next line, **including `\n`** if one exists
- `NULL` if the end of file is reached or an error occurs

**The caller is responsible for `free()`-ing the returned string.**

---

### Helper Functions (in `get_next_line_utils.c`)

These are standalone re-implementations — the standard library is not used.

| Function        | Prototype                                         | Description                                                        |
|-----------------|---------------------------------------------------|--------------------------------------------------------------------|
| `ft_strlen`     | `size_t ft_strlen(const char *s)`                | Returns string length; returns `0` if `s` is `NULL`               |
| `ft_strchr`     | `char *ft_strchr(const char *s, int c)`           | Finds first occurrence of `c` in `s`; handles `'\0'` correctly    |
| `ft_strjoin`    | `char *ft_strjoin(char *s1, char *s2)`            | Concatenates `s1` + `s2` into a new allocation; **frees `s1`**    |
| `gnl_free_rest` | `char *gnl_free_rest(char **rest)`                | Frees and NULLs the `*rest` pointer; returns `NULL`                |

> ⚠️ **Note:** `ft_strjoin` here differs from the libft version — it **frees `s1`** after joining. This is intentional: the accumulator (`rest`) is always replaced by the new joined string, so the old one must be freed immediately to avoid leaks.

---

## 🧠 How It Works — Internal Architecture

The function is broken into four static helper functions inside `get_next_line.c`:

### 1. `read_join(int fd, char *rest)` — Fill the buffer

```
reads from fd in chunks of BUFFER_SIZE bytes
→ appends each chunk to `rest` via ft_strjoin
→ stops when a '\n' is found in `rest` OR end-of-file is reached
→ returns the accumulated string (or NULL on error)
```

This is the only place where `read()` is called. The loop reads until it finds a newline in the accumulated `rest`, so it never over-reads more than one `BUFFER_SIZE` chunk past the newline.

### 2. `extract_line(char **rest)` — Carve out one line

```
scans *rest for '\n'
→ allocates a new string: everything up to and including '\n'
→ calls new_rest() to save everything after '\n' back into *rest
→ returns the line
```

### 3. `new_rest(char *rest, int i)` — Save the leftover

```
after a line is extracted, the data after '\n' is not discarded
→ it is saved into a new allocation and stored back in `rest`
→ this is what gets used on the NEXT call to get_next_line
```

### 4. `free_all(char *a, char *b)` — Cleanup on error

```
frees both pointers and returns NULL
→ used to clean up in a single return statement when read() fails or malloc fails
```

### Call flow diagram

```
get_next_line(fd)
    │
    ├─► read_join(fd, rest)
    │       │
    │       ├── read() → buf → ft_strjoin(rest, buf) → rest
    │       ├── loop until '\n' found in rest OR EOF
    │       └── return accumulated rest
    │
    └─► extract_line(&rest)
            │
            ├── find '\n' position in rest
            ├── malloc line (up to and including '\n')
            ├── new_rest() → save tail after '\n' back into rest
            └── return line
```

---

## ⭐ Bonus — Multiple File Descriptors

The bonus version handles **multiple open file descriptors at the same time**, each maintaining its own independent reading position and buffer.

### Key difference in `get_next_line_bonus.c`

**Mandatory:**
```c
static char *rest;           // one single shared buffer
```

**Bonus:**
```c
static char *rest[10240];    // one buffer per fd (up to fd 10240)
```

Each file descriptor index maps directly to its own `rest` slot. This means you can interleave calls across different fds and each one remembers exactly where it left off.

```c
// Example: reading 3 files alternately
line1 = get_next_line(fd1);  // reads from file1's independent buffer
line2 = get_next_line(fd2);  // reads from file2's independent buffer
line3 = get_next_line(fd3);  // reads from file3's independent buffer
```

### Bounds check

The bonus version adds an additional guard:

```c
if (fd < 0 || fd > 10240 || BUFFER_SIZE <= 0)
    return (NULL);
```

This prevents out-of-bounds access on the `rest` array.

---

## 📋 Edge Cases Handled

| Scenario                              | Behaviour                                                   |
|---------------------------------------|-------------------------------------------------------------|
| `BUFFER_SIZE = 1`                     | Reads one byte at a time; still correct                     |
| `BUFFER_SIZE` larger than entire file | Reads file in one shot; extracts lines from memory          |
| File with no trailing `\n`            | Last line is returned without `\n`; next call returns `NULL`|
| Empty file                            | First call returns `NULL`                                   |
| `fd < 0` (invalid fd)                 | Returns `NULL` immediately                                  |
| `BUFFER_SIZE <= 0`                    | Returns `NULL` immediately                                  |
| `read()` error                        | Returns `NULL`, frees allocated memory (`free_all`)         |
| `malloc` failure mid-read             | Returns `NULL`, all intermediate memory is freed            |
| Reading from `stdin` (fd = 0)         | Works exactly like reading from a file                      |
| Multiple fds (bonus)                  | Each fd has its own independent `rest` buffer               |

---

## 🧪 Testing

### Using the provided `main.c` (single fd)

Create a test file:
```bash
printf "Line one\nLine two\nLine three without newline" > test.txt
```

Compile and run:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o test && ./test
```

Expected output:
```
=== TEST get_next_line SUR UN SEUL FICHIER ===

Lecture du fichier test.txt...

Ligne 1 : "Line one\n"
  -> Ligne standard (terminée par '\n')
Ligne 2 : "Line two\n"
  -> Ligne standard (terminée par '\n')
Ligne 3 : "Line three without newline"
  -> Dernière ligne (pas de '\n' à la fin)

=== Fin du fichier atteinte ===
```

### Using the provided `main_bonus.c` (multiple fds)

```bash
echo -e "A1\nA2\nA3" > file1.txt
echo -e "B1\nB2\nB3" > file2.txt
echo -e "C1\nC2\nC3" > file3.txt

cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main_bonus.c -o test_bonus && ./test_bonus
```

### Stress testing `BUFFER_SIZE`

```bash
for bs in 1 2 5 42 100 9999 100000; do
    cc -D BUFFER_SIZE=$bs get_next_line.c get_next_line_utils.c main.c -o test_$bs && ./test_$bs
done
```

### Check for memory leaks

```bash
valgrind --leak-check=full --track-origins=yes ./test
```

### Popular third-party testers

- [gnlTester](https://github.com/Tripouille/gnlTester)
- [gnl-station-42](https://github.com/kodpe/gnl-station-42)
- [francinette](https://github.com/xicodomingues/francinette)

---

## ✅ 42 Norm Compliance

This project follows the **42 Norm** (Norminette):

- Maximum **25 lines** per function
- No more than **5 variables** declared per function
- No `for` loops (only `while`)
- No assignments inside conditions
- Proper header guards in all `.h` files
- `BUFFER_SIZE` defined via `#ifndef` guard so it can be overridden at compile time

---

## 📌 Important Notes for 42 Students

- **You must `free()` every line** returned by `get_next_line` — the caller owns the memory.
- **The `static` variable persists** between calls within the same program run. When the file ends, the leftover buffer in `rest` is cleaned up naturally via `gnl_free_rest`.
- **`ft_strjoin` frees `s1`** — this is a deliberate design choice to avoid leaks in the read loop. Do not confuse it with the `libft` version which does not free its arguments.
- **`BUFFER_SIZE` can be anything** — your evaluator will test `BUFFER_SIZE=1` and very large values. Never assume a specific buffer size.
- **Do not use `lseek()`** or any function that repositions the file offset — only `read()` is permitted.
- **Global variables are forbidden** — the `static` local variable inside `get_next_line` is the correct approach.
- The bonus `rest[10240]` array is statically allocated — it does not need to be `free`d explicitly, but each `rest[fd]` slot must be freed when done reading.

---

*Created by mel-hyna — 42 School, 2025*
