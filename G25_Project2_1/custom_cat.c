#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * custom_cat.c
 * Author: Nayan Kumar Choudhary 
 * Roll No: 23je0636
 * Group: G25
 *
 * Description:
 * A custom implementation of the Unix 'cat' utility.
 * Supports displaying file contents, numbering lines,
 * concatenating multiple files, and reading from stdin.
 *
 * Supported flags:
 *   -n   : prefix each line with its line number
 *
 * Usage:
 *   ./custom_cat file.txt
 *   ./custom_cat -n file.txt
 *   ./custom_cat file1.txt file2.txt
 *   echo "hello" | ./custom_cat
 */

/* -------------------------------------------------------
 * show_contents()
 * Reads from a file pointer line by line and prints it.
 * If flag_numbering is set, each line is prefixed with
 * its line number (right-aligned in a field of width 4).
 * ------------------------------------------------------- */
void show_contents(FILE *src, int flag_numbering) {
    int counter = 1;
    int ch;
    int at_line_start = 1;  /* tracks if we are at beginning of a new line */

    while ((ch = fgetc(src)) != EOF) {
        if (at_line_start && flag_numbering) {
            printf("%4d | ", counter++);
            at_line_start = 0;
        }
        putchar(ch);
        if (ch == '\n') {
            at_line_start = 1;
        }
    }
}

/* -------------------------------------------------------
 * open_and_display()
 * Opens a file by name and passes it to show_contents().
 * Handles error if file cannot be opened.
 * ------------------------------------------------------- */
int open_and_display(const char *filename, int flag_numbering) {
    FILE *src = fopen(filename, "r");
    if (src == NULL) {
        fprintf(stderr, "custom_cat: cannot open '%s': ", filename);
        perror("");
        return -1;   /* signal failure to caller */
    }
    show_contents(src, flag_numbering);
    fclose(src);
    return 0;
}

/* -------------------------------------------------------
 * parse_flags()
 * Scans argv for supported flags and returns the index
 * at which actual file arguments begin.
 * ------------------------------------------------------- */
int parse_flags(int argc, char *argv[], int *flag_numbering) {
    int idx = 1;
    while (idx < argc && argv[idx][0] == '-') {
        if (strcmp(argv[idx], "-n") == 0) {
            *flag_numbering = 1;
        } else {
            fprintf(stderr, "custom_cat: unknown flag '%s'\n", argv[idx]);
        }
        idx++;
    }
    return idx;  /* first non-flag argument position */
}

/* -------------------------------------------------------
 * main()
 * Entry point. Parses arguments, handles stdin fallback,
 * and iterates over each file provided.
 * ------------------------------------------------------- */
int main(int argc, char *argv[]) {
    int flag_numbering = 0;
    int exit_status = 0;

    /* Parse any flags from command line */
    int file_idx = parse_flags(argc, argv, &flag_numbering);

    /* If no file arguments remain, read from standard input */
    if (file_idx >= argc) {
        show_contents(stdin, flag_numbering);
        return 0;
    }

    /* Process every file provided as argument */
    for (int i = file_idx; i < argc; i++) {
        int result = open_and_display(argv[i], flag_numbering);
        if (result != 0) {
            exit_status = 1;  /* mark failure but continue remaining files */
        }
    }

    return exit_status;
}
