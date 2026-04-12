#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void count_file(FILE *fp, int show_l, int show_w, int show_c) {
    int ch;
    int lines = 0, words = 0, chars = 0;
    int in_word = 0;

    while((ch = fgetc(fp)) != EOF){
        chars++;

        if(ch == '\n') lines++;

        if(isspace(ch)){
            in_word = 0;
        } else if(!in_word){
            in_word = 1;
            words++;
        }
    }

    if(show_l) printf("%d ", lines);
    if(show_w) printf("%d ", words);
    if(show_c) printf("%d ", chars);
}

int main(int argc, char *argv[]) {
    int show_l = 1, show_w = 1, show_c = 1;
    int i = 1;

    // Parse flags
    while(i < argc && argv[i][0] == '-'){
        show_l = show_w = show_c = 0;
        for(int j = 1; argv[i][j]; j++){
            if(argv[i][j] == 'l') show_l = 1;
            if(argv[i][j] == 'w') show_w = 1;
            if(argv[i][j] == 'c') show_c = 1;
        }
        i++;
    }

    // No file → stdin
    if(i == argc){
        count_file(stdin, show_l, show_w, show_c);
        printf("\n");
        return 0;
    }

    // Multiple files
    for(; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        if(!fp){
            perror(argv[i]);
            continue;
        }

        count_file(fp, show_l, show_w, show_c);
        printf("%s\n", argv[i]);
        fclose(fp);
    }

    return 0;
}
