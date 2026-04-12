#include <stdio.h>
#include <unistd.h>
#include <errno.h>
/*
 * custom_mv.c
 * Author: Md Shadab
 * Admission Number: 23JE0576
 * Group: G25
 *
 * Description:
 * A custom implementation of the Unix 'mv' (move) utility.
 * It renames or moves a file from one path to another using 
 * the rename() system call.
 *
 * Usage:
 * ./custom_mv old_name.txt new_name.txt
 *
 * Inside custom_shell:
 * custom_mv old_name.txt new_name.txt
 */
int main(int argc, char *argv[]) {
    // 1. Check if the user provided both source and destination
    if (argc != 3) {
        printf("Usage: %s <source_file> <destination_path>\n", argv[0]);
        return 1;
    }

    // 2. Use the rename system call
    // This moves the file from argv[1] to argv[2]
    if (rename(argv[1], argv[2]) == 0) {
        printf("File moved/renamed successfully from %s to %s\n", argv[1], argv[2]);
    } else {
        // 3. If rename fails, perror prints the specific system error
        perror("Error moving file");
        return 1;
    }

    return 0;
}