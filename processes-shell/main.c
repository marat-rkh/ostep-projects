#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG_ENABLED 0

#if DEBUG_ENABLED
#define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: " fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...) ((void)0)
#endif

void reportError() {
    fprintf(stderr, "An error has occurred\n");
}

int readUserInput(char** linePtr) {
    char* line = NULL;
    size_t linecapp = 0;
    if (getline(&line, &linecapp, stdin) < 0) {
        return 1;
    }
    size_t lineLen = strlen(line);
    if (lineLen > 0 && line[lineLen - 1] == '\n') {
        line[lineLen - 1] = '\0';
    }
    DEBUG("line len: %ld, line: %s\n", lineLen, line);
    *linePtr = line;
    return 0;
}

int tokenize(char*** tokensPtr, size_t* tokensLenPtr, char* line) {
    size_t tokensLen = 0, tokensCap = 8;
    char** tokens = malloc(tokensCap * sizeof(char*));
    if (!tokens) {
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
                free(tokens);
                return 1;
            }
            tokens = tmp;
        }
        tokens[tokensLen++] = token;
    }
    DEBUG("tokens count: %ld, first token: %s\n", tokensLen, tokens[0]);
    *tokensPtr = tokens;
    *tokensLenPtr = tokensLen;
    return 0;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        // batch mode
    }
    while (true) {
        printf("wish> ");
        char* line = NULL;
        if (readUserInput(&line) != 0) {
            reportError();
            return 1;
        }
        char** tokens = NULL;
        size_t tokensLen = 0;
        if (tokenize(&tokens, &tokensLen, line) != 0) {
            reportError();
            free(line);
            return 1;
        }
        
        if (tokensLen == 1 && strcmp(tokens[0], "exit") == 0) {
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