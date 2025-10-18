#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void reportError() {
    fprintf(stderr, "An error has occurred\n");
}

int main(int argc, char** argv) {
    if (argc > 1) {
        // batch mode
    }
    while (true) {
        printf("wish> ");
        char* line = NULL;
        size_t lineLen = 0;
        if (getline(&line, &lineLen, stdin) < 0) {
            reportError();
            return 1;
        }
        size_t tokensLen = 0, tokensCap = 8;
        char** tokens = malloc(tokensCap * sizeof(char*));
        if (!tokens) {
            reportError();
            free(line);
            return 1;
        }
        char* token;
        char* next = line;
        while ((token = strsep(&next, " \t")) != NULL) {
            if (*token == '\0') {
                continue;
            }
            if (tokensLen == tokensCap) {
                tokensCap *= 2;
                char** tmp = realloc(tokens, tokensCap * sizeof(char*));
                if (!tmp) {
                    reportError();
                    free(tokens);
                    free(line);
                    return 1;
                }
                tokens = tmp;
            }
            tokens[tokensLen++] = token;
        }
        // TODO our last token seems to end with newline, this why code doesn't work
        printf("DEBUG: tokens count: %ld, first token: %s\n", tokensLen, tokens[0]);
        if (tokensLen == 0 && strcmp(tokens[0], "exit") == 0) {
            free(tokens);
            free(line);
            return 0;
        } else {
            int rcFork = fork();
            if (rcFork < 0) {
                reportError();
                free(tokens);
                free(line);
                return 1;
            } else if (rcFork == 0) {
                execvp(tokens[0], tokens);
            }
            wait(NULL);
        }

        free(tokens);
        free(line);
    }
    return 0;
}