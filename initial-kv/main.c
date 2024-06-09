#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Cmd {
    char *cmd;
    char *key;
    char *value;
    char *arg;
};

struct Cmd *parse_cmds(char const* args[], int args_len) {
    if (args_len == 0) {
        return NULL;
    }
    struct Cmd *cmds = malloc(args_len * sizeof(struct Cmd));
    for (int i = 0; i < args_len; i++) {
        cmds[i].arg = strdup(args[i]);
        char *arg = cmds[i].arg;
        cmds[i].cmd = strsep(&arg, ",");
        cmds[i].key = strsep(&arg, ",");
        cmds[i].value = strsep(&arg, ",");
        if (arg != NULL) {
            cmds[i].cmd = NULL;
            cmds[i].key = NULL;
            cmds[i].value = NULL;
        }
    }
    return cmds;
}

void free_cmds(struct Cmd *cmds, int cmds_len) {
    for (int i = 0; i < cmds_len; i++) {
        free(cmds[i].arg);
    }
    free(cmds);
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        return 0;
    }
    const int cmds_len = argc - 1;
    struct Cmd *cmds = parse_cmds(argv + 1, cmds_len); 
    for (int i = 0; i < cmds_len; i++) {
        struct Cmd cmd = cmds[i];
        if (cmd.cmd == NULL) {
            printf("bad command\n");
        } else if (strcmp(cmd.cmd, "p") == 0 && cmd.key != NULL && cmd.value != NULL) {
            printf("PUT %s %s\n", cmd.key, cmd.value);
        } else if (strcmp(cmd.cmd, "g") == 0 && cmd.key != NULL && cmd.value == NULL) {
            printf("GET %s\n", cmd.key);
        } else if (strcmp(cmd.cmd, "d") == 0 && cmd.key != NULL && cmd.value == NULL) {
            printf("DEL %s\n", cmd.key);
        } else if (strcmp(cmd.cmd, "c") == 0 && cmd.key == NULL && cmd.value == NULL) {
            printf("CLEAR\n");
        } else if (strcmp(cmd.cmd, "a") == 0 && cmd.key == NULL && cmd.value == NULL) {
            printf("ALL\n");
        } else {
            printf("bad command\n");
        }
    }
    free_cmds(cmds, cmds_len);
    return 0;
}