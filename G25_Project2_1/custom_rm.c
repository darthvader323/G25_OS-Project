#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int interactive = 0;
    int i = 1;

    if(argc < 2){
        printf("Usage: custom_rm [-i] <file1> <file2> ...\n");
        return 1;
    }

    // flag parsing
    if(strcmp(argv[1], "-i") == 0){
        interactive = 1;
        i++;
    }

    for(; i < argc; i++){
        if(interactive){
            char choice;
            printf("Remove %s? (y/n): ", argv[i]);
            scanf(" %c", &choice);
            if(choice != 'y') continue;
        }

        if(remove(argv[i]) == 0){
            printf("Deleted: %s\n", argv[i]);
        } else {
            perror(argv[i]);
        }
    }

    return 0;
}
