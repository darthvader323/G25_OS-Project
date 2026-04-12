***********************************************************INTRODUCTION***********************************************************

This project focuses on developing a set of simplified UNIX-like utilities in C, along with a custom-built command-line shell. The objective is to understand how common UNIX commands such as file handling, directory listing, searching, and process execution work internally by implementing lightweight versions from scratch. Instead of using standard commands, each utility is implemented with a custom prefix (e.g., custom_ls, custom_cat) to ensure originality and avoid conflicts with existing system tools.

A key component of the project is the custom shell, which acts as the interface between the user and the implemented utilities. The shell reads user commands, parses them, and executes the corresponding programs using system calls like fork() and exec(). This provides hands-on experience with process creation and command execution in a UNIX-like environment.

**********************************************************DIVISION OF WORK**********************************************************

To ensure efficient development and equal contribution, the project has been divided among six team members. Each member is responsible for implementing specific utilities and preparing their own documentation, including code explanation and execution screenshots.

Adyant: custom_shell (main shell + command execution

Nayan:  custom_cat

Sahil:  custom_grep

Shadab: custom_cp, custom_mv

Prabhu: custom_rm, custom_wc

Riyan:  custom_ls

In addition to their assigned utilities, all members contribute to updating the Makefile and README, ensuring consistency across the project. Each member also documents their implementation independently, highlighting their understanding of system calls and file handling in C.

********************************************************OBJECTIVE********************************************************

The project aims to:

1. Build a foundational understanding of UNIX command behavior
2. Gain practical experience with system calls in C
3. Develop modular and collaborative programming skills
4. Simulate a real-world command-line environment









*************************************************************CUSTOM SHELL*************************************************************

DONE BY: Adyant Kar (22JE0067)

A lightweight interactive UNIX-like shell written from scratch in C, built as part of a group OS assignment. This shell serves as the main program that ties together all the custom utilities developed by the team.

---

## What It Does

`custom_shell` is an interactive command-line shell that:
- Displays a **coloured prompt** showing the current directory (with `~` shorthand for home)
- Handles **built-in commands** directly inside the shell process
- Runs **external commands and custom utilities** by forking a child process and using `execvp`
- Automatically **resolves custom utility binaries** from the same directory as the shell, so all teammates' utilities work seamlessly when placed together

-------------------------------------------------------------------------------------------------------------------------------------------

## Built-in Commands

| Command | Description |


| `cd [dir]` | Change directory. With no argument, goes to `$HOME` |


| `pwd` | Print the current working directory |


| `help` | Display all available commands |


| `exit` / `quit` | Exit the shell |

---------------------------------------------------------------------------------------------------------------------------------------------

## How to Build

Requirements: GCC, Make, Linux or WSL (Windows Subsystem for Linux)

```bash
# Build only the shell
gcc -Wall -Wextra -std=c11 -O2 -o custom_shell custom_shell.c

# Or build everything with the team's Makefile
make
```

## How to Run

```bash
./custom_shell
```

You will see:
```
Welcome to Custom Shell! Type 'help' for commands.
custom_shell:~/your-directory$
```

Type `help` to list all commands. Press **Ctrl+D** or type `exit` to quit.

--------------------------------------------------------------------------------------------------------------------------------------------------

## Implementation Details

### Input Parsing
User input (up to 1024 characters) is read with `fgets` and tokenised using `strtok` on spaces and tabs, supporting up to 64 arguments per command.

### Built-in Execution
`cd`, `pwd`, `help`, `exit`, and `quit` are handled directly in the shell process without forking, since they need to affect the shell's own state (e.g. the working directory).

### External Command Execution
All other commands go through `fork()` + `execvp()`. The parent waits for the child to finish using `waitpid()` before returning to the prompt.

### Utility Resolution
When a custom utility name is detected (e.g. `custom_ls`, `custom_grep`), the shell resolves its path in this order:
1. Same directory as the running shell binary (`/proc/self/exe`)
2. Current working directory (`./`)
3. Standard `PATH` environment variable

This means all teammates' utilities just need to be in the same folder — no PATH setup required.

### Prompt
The prompt is formatted as `custom_shell:<cwd>$` with ANSI colours (green for the shell name, cyan for the path). If the current directory is inside `$HOME`, the home prefix is replaced with `~`.

-----------------------------------------------------------------------------------------------------------------------------------------------

## File

| File | Description |


| `custom_shell.c` | Complete shell implementation — single self-contained file |

------------------------------------------------------------------------------------------------------------------------------------------------

## Notes
- The shell is intentionally minimal — no piping, redirection, or job control — to keep scope focused on the core shell loop and utility integration.





************************************************************CUSTOM CAT************************************************************

DONE BY: Nayan Kumar Choudhary (23J30636)

A lightweight implementation of the Unix `cat` utility written from scratch in C, built as part of a group OS assignment. This utility is executed through the custom shell developed by the team.

---

## What It Does

`custom_cat` is a file display utility that:
- Reads and displays the contents of one or more files to standard output
- Numbers each output line when the `-n` flag is provided
- Concatenates multiple files and prints them sequentially
- Falls back to reading from **standard input** if no file is provided
- Handles missing or unreadable files gracefully with a descriptive error message

 ---

 ## Supported Flag

 | Flag | Description |
|------|-------------|
| `-n` | Prefix each output line with its line number |

---

## How to Build

Requirements: GCC, Linux or WSL (Windows Subsystem for Linux)

```bash
# Build only custom_cat
gcc -Wall -g -o custom_cat custom_cat.c

# Or build everything with the team's Makefile
make
```
## How to Run

```bash
./custom_cat file.txt
```

Or inside the custom shell:
```bash
./custom_shell
custom_cat file.txt
```

---

## Usage Examples
```bash
# Display a file
custom_cat file.txt

# Display with line numbers
custom_cat -n file.txt

# Concatenate two files
custom_cat file1.txt file2.txt

# Read from stdin
echo "hello" | custom_cat

# Error handling for missing file
custom_cat fakefile.txt
```
---
## Implementation Details

### Input Handling
The utility reads input **character by character** using `fgetc()`, which gives precise control over line boundary detection. This differs from the common `fgets()` approach and allows accurate line numbering even for edge cases.

### Modular Design
The implementation is split into three focused functions:

| Function | Description |
|----------|-------------|
| `show_contents()` | Reads from a file pointer char by char, prints content, handles line numbering using an `at_line_start` flag |
| `open_and_display()` | Safely opens a file by name, calls `show_contents()`, handles file open errors using `perror()` |
| `parse_flags()` | Scans command line arguments for supported flags and returns the index where file arguments begin |

### Error Handling
If a file cannot be opened, the utility prints a descriptive error message using both `fprintf(stderr)` and `perror()` for OS-level detail, then continues processing any remaining files instead of exiting immediately. The final exit status reflects whether any errors occurred.

### Stdin Fallback
If no file arguments are provided after flag parsing, `custom_cat` automatically reads from standard input, matching standard Unix `cat` behaviour.

---
## File

| File | Description |
|------|-------------|
| `custom_cat.c` | Complete custom_cat implementation — single self-contained file |

---
## Notes
- All logic is implemented from scratch in C with no external libraries
- Integrates seamlessly with `custom_shell` since the shell uses `execvp()` to resolve binaries from the same directory
- Tested on Ubuntu (Linux / WSL)

 




