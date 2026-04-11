#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024

// Function to convert string to lowercase
void to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Case-insensitive substring check
int contains_ignore_case(char *line, char *pattern) {
    char temp_line[MAX_LINE];
    char temp_pattern[MAX_LINE];

    strcpy(temp_line, line);
    strcpy(temp_pattern, pattern);

    to_lower(temp_line);
    to_lower(temp_pattern);

    return strstr(temp_line, temp_pattern) != NULL;
}

int main(int argc, char *argv[]) {
    int ignore_case = 0;
    char *pattern;
    char *filename;

    // Argument handling
    if (argc == 3) {
        pattern = argv[1];
        filename = argv[2];
    }
    else if (argc == 4 && strcmp(argv[1], "-i") == 0) {
        ignore_case = 1;
        pattern = argv[2];
        filename = argv[3];
    }
    else {
        printf("Usage:\n");
        printf("  %s <pattern> <filename>\n", argv[0]);
        printf("  %s -i <pattern> <filename>\n", argv[0]);
        return 1;
    }

    // Open file
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE];
    int line_no = 1;

    // Read file line by line
    while (fgets(line, sizeof(line), file)) {

        int match;

        if (ignore_case) {
            match = contains_ignore_case(line, pattern);
        } else {
            match = strstr(line, pattern) != NULL;
        }

        if (match) {
            printf("%d: %s", line_no, line);
        }

        line_no++;
    }

    fclose(file);
    return 0;
}