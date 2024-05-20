#include <stdio.h>

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }
    int lastChar = EOF;
    int currentChar = EOF;
    int count = -1;
    for (int i = 1; i < argc; i++) {
        const char* filename = argv[i];
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            printf("wzip: cannot open file '%s'\n", filename);
            return 1;
        }
        if (lastChar == EOF) {
            currentChar = fgetc(file);
            count = 1;
        } else {
            currentChar = lastChar;
        }
        while (currentChar != EOF) {
            int nextChar = fgetc(file);
            if (nextChar == currentChar) {
                ++count;
            } else {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&currentChar, sizeof(char), 1, stdout);
                count = 1;
            }
            lastChar = currentChar;
            currentChar = nextChar;
        }
        fclose(file);
    }
    if (count > 1) {
        fwrite(&count, sizeof(int), 1, stdout);
        fwrite(&lastChar, sizeof(char), 1, stdout);
    }
    return 0;
}