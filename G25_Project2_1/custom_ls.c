#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_FILES 1024   // max number of files we will store from a directory

/* Color codes (for better terminal output, like real ls) */
#define COLOR_RESET  "\033[0m"
#define COLOR_BLUE   "\033[1;34m"   // directories
#define COLOR_GREEN  "\033[1;32m"   // executables

/* ------------------------------------------------------- */
/* Prints file permissions in rwx format (just like ls -l) */
void print_permissions(mode_t mode) {
    printf(S_ISDIR(mode) ? "d" : "-");   // check if directory

    // owner permissions
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    // group permissions
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    // others permissions
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

/* ------------------------------------------------------- */
/* Prints file name with color depending on type */
void print_colored_name(const char *name, mode_t mode) {
    if (S_ISDIR(mode)) {
        // directories in blue
        printf(COLOR_BLUE "%s" COLOR_RESET, name);
    } else if (mode & S_IXUSR) {
        // executable files in green
        printf(COLOR_GREEN "%s" COLOR_RESET, name);
    } else {
        // normal files
        printf("%s", name);
    }
}

/* ------------------------------------------------------- */
/* Converts file size into human readable form (B, KB, MB) */
void print_size(long size) {
    if (size < 1024)
        printf("%ldB", size);
    else if (size < 1024 * 1024)
        printf("%.1fK", size / 1024.0);
    else
        printf("%.1fM", size / (1024.0 * 1024));
}

/* ------------------------------------------------------- */
/* Used for sorting file names alphabetically */
int compare(const void *a, const void *b) {
    struct dirent *f1 = *(struct dirent **)a;
    struct dirent *f2 = *(struct dirent **)b;
    return strcmp(f1->d_name, f2->d_name);
}

/* ------------------------------------------------------- */
/* Main logic: open directory, read files, sort and print */
void list_directory(const char *path, int flag_l, int flag_a, int flag_h) {
    DIR *dir;
    struct dirent *entry;
    struct dirent *files[MAX_FILES];

    int n = 0;   // number of files collected
    int file_count = 0, dir_count = 0;  // for summary

    dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "custom_ls: cannot access '%s'\n", path);
        return;
    }

    /* Read all entries from directory */
    while ((entry = readdir(dir)) != NULL) {
        // skip hidden files unless -a is given
        if (!flag_a && entry->d_name[0] == '.') continue;

        files[n++] = entry;   // store entry for sorting later
    }

    closedir(dir);

    /* Sort entries alphabetically */
    qsort(files, n, sizeof(struct dirent *), compare);

    /* Print each file */
    for (int i = 0; i < n; i++) {
        char full_path[512];
        struct stat fileStat;

        // build full path (needed for stat)
        snprintf(full_path, sizeof(full_path), "%s/%s", path, files[i]->d_name);

        if (stat(full_path, &fileStat) == -1) {
            perror("stat");
            continue;
        }

        // count files vs directories
        if (S_ISDIR(fileStat.st_mode))
            dir_count++;
        else
            file_count++;

        if (flag_l) {
            // long listing format
            print_permissions(fileStat.st_mode);

            printf(" %ld", fileStat.st_nlink);   // number of links

            printf(" ");
            if (flag_h)
                print_size(fileStat.st_size);   // human readable size
            else
                printf("%5ld", fileStat.st_size);  // normal size

            // format time nicely
            char timebuf[64];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M",
                     localtime(&fileStat.st_mtime));

            printf(" %s ", timebuf);

            // print name with color
            print_colored_name(files[i]->d_name, fileStat.st_mode);
            printf("\n");
        } else {
            // simple listing
            print_colored_name(files[i]->d_name, fileStat.st_mode);
            printf("\n");
        }
    }

    /* Print summary at end */
    printf("\nFiles: %d | Directories: %d\n", file_count, dir_count);
}

/* ------------------------------------------------------- */
/* Handles flags like -l, -a, -h and also combined ones (-lah) */
int parse_flags(int argc, char *argv[], int *flag_l, int *flag_a, int *flag_h) {
    int idx = 1;

    while (idx < argc && argv[idx][0] == '-') {
        // go through each character after '-'
        for (int j = 1; argv[idx][j] != '\0'; j++) {
            if (argv[idx][j] == 'l')
                *flag_l = 1;
            else if (argv[idx][j] == 'a')
                *flag_a = 1;
            else if (argv[idx][j] == 'h')
                *flag_h = 1;
            else
                fprintf(stderr, "custom_ls: invalid option -- '%c'\n", argv[idx][j]);
        }
        idx++;
    }

    return idx;  // index where directory names start
}

/* ------------------------------------------------------- */
/* Entry point of program */
int main(int argc, char *argv[]) {
    int flag_l = 0, flag_a = 0, flag_h = 0;

    // parse flags first
    int dir_index = parse_flags(argc, argv, &flag_l, &flag_a, &flag_h);

    // if no directory provided → use current directory
    if (dir_index >= argc) {
        list_directory(".", flag_l, flag_a, flag_h);
    } else {
        // handle multiple directories
        for (int i = dir_index; i < argc; i++) {
            list_directory(argv[i], flag_l, flag_a, flag_h);
        }
    }

    return 0;
}
