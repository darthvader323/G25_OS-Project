#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
/*
 * custom_cp.c
 * Author: Md Shadab
 * Admission Number: 23JE0576
 * Group: G25
 *
 * Description:
 * A custom implementation of the Unix 'cp' (copy) utility.
 * It copies the content of a source file to a destination file
 * using low-level system calls (open, read, write).
 *
 * Usage:
 * ./custom_cp source.txt destination.txt
 *
 * Inside custom_shell:
 * custom_cp source.txt destination.txt
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        return 1;
    }

    // Create dest file: Write-only, Create if missing, Truncate if exists, Permissions 0644
    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Error creating destination file");
        close(src_fd);
        return 1;
    }

    char buffer[4096];
    ssize_t n_read;
    while ((n_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, n_read) != n_read) {
            perror("Error writing to destination");
            break;
        }
    }

    close(src_fd);
    close(dest_fd);
    printf("File copied successfully.\n");
    return 0;
}