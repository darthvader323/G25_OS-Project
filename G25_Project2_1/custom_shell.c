/*
 * custom_shell.c
 * A simplified UNIX-like shell that supports built-in commands
 * and external custom utilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_INPUT   1024
#define MAX_ARGS    64
#define MAX_PATH    512

/* ANSI color codes */
#define COLOR_RESET  "\033[0m"
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_CYAN   "\033[1;36m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_RED    "\033[1;31m"

/* ------------------------------------------------------------------ */
/* Built-in: cd                                                        */
/* ------------------------------------------------------------------ */
static int builtin_cd(char **args)
{
    if (args[1] == NULL) {
        const char *home = getenv("HOME");
        if (home == NULL) home = "/";
        if (chdir(home) != 0) {
            perror("custom_shell: cd");
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("custom_shell: cd");
        }
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* Built-in: pwd                                                       */
/* ------------------------------------------------------------------ */
static int builtin_pwd(void)
{
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("custom_shell: pwd");
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* Built-in: help                                                      */
/* ------------------------------------------------------------------ */
static int builtin_help(void)
{
    printf(COLOR_CYAN
           "╔══════════════════════════════════════════════════╗\n"
           "║           Custom UNIX-like Shell                ║\n"
           "║         Supported Commands                      ║\n"
           "╚══════════════════════════════════════════════════╝\n"
           COLOR_RESET);
    printf(COLOR_YELLOW "Built-in commands:\n" COLOR_RESET);
    printf("  cd [dir]    - Change directory\n");
    printf("  pwd         - Print working directory\n");
    printf("  help        - Show this help message\n");
    printf("  exit / quit - Exit the shell\n\n");
    printf(COLOR_YELLOW "Custom utilities:\n" COLOR_RESET);
    printf("  custom_ls   [dir]              - List directory contents\n");
    printf("  custom_cat  <file> [file...]   - Display file contents\n");
    printf("  custom_grep <pattern> <file>   - Search pattern in file\n");
    printf("  custom_wc   <file> [file...]   - Word/line/char count\n");
    printf("  custom_cp   <src> <dst>        - Copy file\n");
    printf("  custom_mv   <src> <dst>        - Move / rename file\n");
    printf("  custom_rm   [-r] <path>        - Remove file or directory\n");
    return 1;
}

/* ------------------------------------------------------------------ */
/* Parse input into argv array; returns number of tokens              */
/* ------------------------------------------------------------------ */
static int parse_input(char *input, char **args)
{
    int count = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[count] = NULL;
    return count;
}

/* ------------------------------------------------------------------ */
/* Resolve utility path: look next to the shell binary, then in PATH  */
/* ------------------------------------------------------------------ */
static int resolve_utility(const char *name, char *out, size_t out_sz)
{
    /* 1. Same directory as the running process */
    char self[MAX_PATH];
    ssize_t len = readlink("/proc/self/exe", self, sizeof(self) - 1);
    if (len > 0) {
        self[len] = '\0';
        char *slash = strrchr(self, '/');
        if (slash) {
            *slash = '\0';
            snprintf(out, out_sz, "%s/%s", self, name);
            if (access(out, X_OK) == 0) return 1;
        }
    }

    /* 2. Current working directory */
    snprintf(out, out_sz, "./%s", name);
    if (access(out, X_OK) == 0) return 1;

    /* 3. PATH */
    const char *path_env = getenv("PATH");
    if (path_env) {
        char path_copy[2048];
        strncpy(path_copy, path_env, sizeof(path_copy) - 1);
        char *dir = strtok(path_copy, ":");
        while (dir) {
            snprintf(out, out_sz, "%s/%s", dir, name);
            if (access(out, X_OK) == 0) return 1;
            dir = strtok(NULL, ":");
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Execute an external command                                         */
/* ------------------------------------------------------------------ */
static int execute(char **args)
{
    /* Check if it's one of our custom utilities and resolve path */
    const char *custom_utils[] = {
        "custom_ls","custom_cat","custom_grep",
        "custom_wc","custom_cp","custom_mv","custom_rm", NULL
    };

    char resolved[MAX_PATH] = {0};
    for (int i = 0; custom_utils[i]; i++) {
        if (strcmp(args[0], custom_utils[i]) == 0) {
            if (!resolve_utility(args[0], resolved, sizeof(resolved))) {
                fprintf(stderr,
                    COLOR_RED "custom_shell: '%s' not found. "
                    "Run 'make' first.\n" COLOR_RESET, args[0]);
                return 1;
            }
            args[0] = resolved;
            break;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("custom_shell: fork");
        return 1;
    }
    if (pid == 0) {
        /* Child */
        execvp(args[0], args);
        fprintf(stderr, COLOR_RED "custom_shell: %s: %s\n" COLOR_RESET,
                args[0], strerror(errno));
        exit(EXIT_FAILURE);
    }
    /* Parent: wait */
    int status;
    waitpid(pid, &status, 0);
    return 1;
}

/* ------------------------------------------------------------------ */
/* Print prompt                                                        */
/* ------------------------------------------------------------------ */
static void print_prompt(void)
{
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) == NULL) strcpy(cwd, "?");

    /* Shorten home dir to ~ */
    const char *home = getenv("HOME");
    char display[MAX_PATH];
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        snprintf(display, sizeof(display), "~%s", cwd + strlen(home));
    } else {
        snprintf(display, sizeof(display), "%s", cwd);
    }

    printf(COLOR_GREEN "custom_shell" COLOR_RESET
           ":" COLOR_CYAN "%s" COLOR_RESET "$ ", display);
    fflush(stdout);
}

/* ------------------------------------------------------------------ */
/* Main loop                                                           */
/* ------------------------------------------------------------------ */
int main(void)
{
    char  input[MAX_INPUT];
    char *args[MAX_ARGS];

    printf(COLOR_CYAN
           "Welcome to Custom Shell! Type 'help' for commands.\n"
           COLOR_RESET);

    while (1) {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break; /* EOF (Ctrl-D) */
        }

        /* Skip blank lines */
        if (input[0] == '\n') continue;

        int argc = parse_input(input, args);
        if (argc == 0) continue;

        /* Built-ins */
        if (strcmp(args[0], "exit") == 0 ||
            strcmp(args[0], "quit") == 0) {
            printf("Goodbye!\n");
            break;
        }
        if (strcmp(args[0], "cd") == 0) { builtin_cd(args); continue; }
        if (strcmp(args[0], "pwd") == 0) { builtin_pwd();   continue; }
        if (strcmp(args[0], "help") == 0){ builtin_help();  continue; }

        /* External */
        execute(args);
    }

    return 0;
}
