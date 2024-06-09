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

struct Entry {
    int key;
    char *value;
    char *entry;
};

void load(FILE *storage, struct Entry **entries_ptr, int *entries_len) {
    *entries_ptr = NULL;
    *entries_len = 0;
    if (storage == NULL) {
        return;
    }
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, storage)) != -1) {
        if (read == 0) {
            continue;
        }
        char *entry = line;
        char *key_str = strsep(&line, ",");
        char *value = line;
        int key = atoi(key_str);
        append_entry(entries_ptr, entries_len, key, value, entry);
    }
}

// TODO implement srinking when size gets too big
void append_entry(struct Entry **entries_ptr, int *entries_len, int key, char *value, char *entry) {
    *entries_ptr = realloc(*entries_ptr, (*entries_len + 1) * sizeof(struct Entry));
    (*entries_ptr)[*entries_len].key = key;
    (*entries_ptr)[*entries_len].value = value;
    (*entries_ptr)[*entries_len].entry = entry;
    ++(*entries_len);
}

void remove_entry(struct Entry **entries_ptr, int *entries_len, int key) {
    for (int i = 0; i < *entries_len; i++) {
        if ((*entries_ptr)[i].key == key) {
            free((*entries_ptr)[i].entry);
            (*entries_ptr)[i].entry = NULL;
            return;
        }
    }
}

void free_entries(struct Entry *entries, int entries_len) {
    for (int i = 0; i < entries_len; i++) {
        free(entries[i].entry);
    }
    free(entries);
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        return 0;
    }

    FILE *storage_file = fopen("kv.txt", "a+");
    if (storage_file == NULL) {
        printf("cannot open storage file\n");
        return 1;
    }
    rewind(storage_file);
    struct Entry *entries = NULL;
    int entries_len = 0;
    load(storage_file, &entries, &entries_len);

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
    // TODO store entries
    free_cmds(cmds, cmds_len);
    free_entries(entries, entries_len);
    fclose(storage_file);
    return 0;
}