#include <stdio.h>

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }
    int file_lastCharOfPrevFile = EOF;
    int counter_count = 0;
    for (int i = 1; i < argc; i++) {
        const char* filename = argv[i];
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            printf("wzip: cannot open file '%s'\n", filename);
            return 1;
        }
        int file_prevChar = EOF;
        if (file_lastCharOfPrevFile == EOF) {
            // This is the first file in the list.
            int firstChar = fgetc(file);
            if (firstChar == EOF) {
                // Empty file, we skip it.
                fclose(file);
                continue;
            }
            file_prevChar = firstChar;
            // This is the first non empty file, so we start the counter.
            counter_count = 1;
        } else {
            file_prevChar = file_lastCharOfPrevFile;
        }
        int file_currChar = fgetc(file);
        while (file_currChar != EOF) {
            if (file_currChar == file_prevChar) {
                ++counter_count;
            } else {
                fwrite(&counter_count, sizeof(int), 1, stdout);
                fwrite(&file_prevChar, sizeof(char), 1, stdout);
                counter_count = 1;
            }
            file_prevChar = file_currChar;
            file_currChar = fgetc(file);
        }
        fclose(file);
        file_lastCharOfPrevFile = file_prevChar;
    }
    if (counter_count > 0) {
        fwrite(&counter_count, sizeof(int), 1, stdout);
        fwrite(&file_lastCharOfPrevFile, sizeof(char), 1, stdout);
    }
    return 0;
}