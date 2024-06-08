#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const* argv[]) {
    for (int i = 1; i < argc; i++) {
        char *arg_copy = strdup(argv[i]);
        char *arg = arg_copy;
        char *cmd = strsep(&arg, ",");
        char *key = strsep(&arg, ",");
        char *value = strsep(&arg, ",");
        // TODO for testing purposes, define a Cmd struct (triple + arg_copy), define parse_cmds to return an arrays of Cmds, and define free_cmds.
        if (arg != NULL || cmd == NULL) {
            printf("bad command\n");
        } else if (strcmp(cmd, "p") == 0 && key != NULL && value != NULL) {
            printf("PUT %s %s\n", key, value);
        } else if (strcmp(cmd, "g") == 0 && key != NULL && value == NULL) {
            printf("GET %s\n", key);
        } else if (strcmp(cmd, "d") == 0 && key != NULL && value == NULL) {
            printf("DEL %s\n", key);
        } else if (strcmp(cmd, "c") == 0 && key == NULL && value == NULL) {
            printf("CLEAR\n");
        } else if (strcmp(cmd, "a") == 0 && key == NULL && value == NULL) {
            printf("ALL\n");
        } else {
            printf("bad command\n");
        }
        free(arg_copy);
    }
    return 0;
}