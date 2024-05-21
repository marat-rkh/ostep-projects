#include <stdio.h>

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        const char* filename = argv[i];
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            printf("wunzip: cannot open file '%s'\n", filename);
            return 1;
        }
        int count;
        while (fread(&count, sizeof(int), 1, file) == 1) {
            char ch;
            if (fread(&ch, sizeof(char), 1, file) != 1) {
                printf("wunzip: wrong file format '%s'\n", filename);
                return 1;
            }
            for (int i = 0; i < count; i++) {
                printf("%c", ch);
            }
        }
        fclose(file);
    }
    return 0;
}