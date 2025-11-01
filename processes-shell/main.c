#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG_ENABLED 1

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

#define PROCESS_CODE_FAILURE 1
#define PROCESS_CODE_ERROR 2
#define PROCESS_CODE_EXIT 3

void freeList(char** headPtr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        free(headPtr[i]);
    }
    free(headPtr);
}

int process(char* line, char*** pathsPtr, size_t* pathsLenPtr) {
    char** tokens = NULL;
    size_t tokensLen = 0;
    if (tokenize(&tokens, &tokensLen, line) != 0) {
        return PROCESS_CODE_FAILURE;
    }
    int procStatus = 0;
    if (tokensLen == 0) {
    } else if (strcmp(tokens[0], "exit") == 0) {
        if (tokensLen != 1) {
            procStatus = PROCESS_CODE_ERROR;
        } else {
            procStatus = PROCESS_CODE_EXIT;
        }
    } else if (strcmp(tokens[0], "cd") == 0) {
        if (tokensLen != 2 || chdir(tokens[1]) != 0) {
            procStatus = PROCESS_CODE_ERROR;
        }
    } else if (strcmp(tokens[0], "path") == 0) {
        freeList(*pathsPtr, *pathsLenPtr);
        char** newPaths = NULL;
        size_t newPathsLen = tokensLen - 1;
        if (newPathsLen) {
            newPaths = malloc(newPathsLen * sizeof(char*));
            if (!newPaths) {
                procStatus = PROCESS_CODE_ERROR;
            } else {
                for (size_t i = 1; i < tokensLen; i++) {
                    newPaths[i - 1] = strdup(tokens[i]);
                }
            }
        }
        *pathsPtr = newPaths;
        *pathsLenPtr = newPathsLen;
    } else {
        int rcFork = fork();
        if (rcFork < 0) {
            procStatus = PROCESS_CODE_FAILURE;
        } else if (rcFork == 0) {
            execvp(tokens[0], tokens);
        } else {
            wait(NULL);
        }
    }
    free(tokens);
    return procStatus;
}

// TODO:
// - Pressing arrows during input breaks the "exit" command
int main(int argc, char** argv) {
    if (argc > 1) {
        // batch mode
    }
    char** paths = malloc(sizeof(char*));
    if (!paths) {
        reportError();
        return 1;
    }
    paths[0] = strdup("/bin");
    size_t pathsLen = 1;
    while (true) {
        printf("wish> ");
        char* line = NULL;
        if (readUserInput(&line) != 0) {
            reportError();
            freeList(paths, pathsLen);
            return 1;
        }
        int procCode = process(line, &paths, &pathsLen);
        free(line);
        if (procCode == PROCESS_CODE_FAILURE || procCode == PROCESS_CODE_ERROR) {
            reportError();
            if (procCode == PROCESS_CODE_FAILURE) {
                freeList(paths, pathsLen);
                return 1;
            }
        } else if (procCode == PROCESS_CODE_EXIT) {
            freeList(paths, pathsLen);
            return 0;
        }
    }
    freeList(paths, pathsLen);
    return 0;
}