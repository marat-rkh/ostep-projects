#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Utils
long str_to_long(const char* num_ptr, int base, bool* ok) {
    char* end;
    long value = strtol(num_ptr, &end, base);
    *ok = end != num_ptr && *end == '\0';
    return value;
}

// Commands
struct Cmd {
    char* cmd;
    char* key;
    char* value;
    char* arg;
};

struct Cmd* parse_cmds(char const* args[], int args_len) {
    if (args_len == 0) {
        return NULL;
    }
    struct Cmd* cmds = malloc(args_len * sizeof(struct Cmd));
    for (int i = 0; i < args_len; i++) {
        cmds[i].arg = strdup(args[i]);
        char* arg = cmds[i].arg;
        cmds[i].cmd = strsep(&arg, ",");
        cmds[i].key = strsep(&arg, ",");
        cmds[i].value = strsep(&arg, ",");
        if (arg != NULL) {
            cmds[i].cmd = NULL;
            cmds[i].key = NULL;
            cmds[i].value = NULL;
            free(cmds[i].arg);
            cmds[i].arg = NULL;
        }
    }
    return cmds;
}

void free_cmds(struct Cmd* cmds, int cmds_len) {
    for (int i = 0; i < cmds_len; i++) {
        free(cmds[i].arg);
        cmds[i].arg = NULL;
    }
    free(cmds);
}

// Entries
struct Entry {
    long key;
    char* value;
    char* entry;
};

// TODO implement srinking when size gets too big
void append_entry(struct Entry** entries_ptr, int* entries_len, long key, char* value, char* entry) {
    *entries_ptr = realloc(*entries_ptr, (*entries_len + 1) * sizeof(struct Entry));
    (*entries_ptr)[*entries_len].key = key;
    (*entries_ptr)[*entries_len].value = value;
    (*entries_ptr)[*entries_len].entry = entry;
    ++(*entries_len);
}

bool remove_entry(struct Entry** entries_ptr, int* entries_len, long key) {
    for (int i = 0; i < *entries_len; i++) {
        if ((*entries_ptr)[i].entry != NULL && (*entries_ptr)[i].key == key) {
            free((*entries_ptr)[i].entry);
            (*entries_ptr)[i].entry = NULL;
            return true;
        }
    }
    return false;
}

struct Entry* find_entry(struct Entry** entries_ptr, int* entries_len, long key) {
    for (int i = 0; i < *entries_len; i++) {
        if ((*entries_ptr)[i].entry != NULL && (*entries_ptr)[i].key == key) {
            return &(*entries_ptr)[i];
        }
    }
    return NULL;
}

void free_entries(struct Entry* entries, int entries_len) {
    for (int i = 0; i < entries_len; i++) {
        free(entries[i].entry);
        entries[i].entry = NULL;
    }
    free(entries);
}

void load(FILE* storage, struct Entry** entries_ptr, int* entries_len) {
    *entries_ptr = NULL;
    *entries_len = 0;
    if (storage == NULL) {
        return;
    }
    char* line = NULL;
    size_t line_cap = 0;
    ssize_t line_len;
    while ((line_len = getline(&line, &line_cap, storage)) > 0) {
        if (line[line_len - 1] == '\n') {
            line[line_len - 1] = '\0';
            --line_len;
        }
        char* entry = line;
        char* key_str = strsep(&line, ",");
        char* value = line;
        bool key_ok;
        long key = str_to_long(key_str, 10, &key_ok);
        if (!key_ok) {
            printf("fatal: incorrect storage format, non integer key detected\n");
            exit(1);
        }
        append_entry(entries_ptr, entries_len, key, value, entry);
        line = NULL;
    }
}

void store(FILE* storage, struct Entry* entries, int entries_len) {
    if (storage == NULL) {
        return;
    }
    for (int i = 0; i < entries_len; i++) {
        if (entries[i].entry != NULL) {
            fprintf(storage, "%ld,%s", entries[i].key, entries[i].value);
            if (i < entries_len - 1) {
                fprintf(storage, "\n");
            }
        }
    }
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        return 0;
    }

    struct Entry* entries = NULL;
    int entries_len = 0;
    FILE* storage_file = fopen("kv.txt", "r");
    if (storage_file != NULL) {
        load(storage_file, &entries, &entries_len);
        fclose(storage_file);
    }

    const int cmds_len = argc - 1;
    struct Cmd* cmds = parse_cmds(argv + 1, cmds_len);
    for (int i = 0; i < cmds_len; i++) {
        struct Cmd cmd = cmds[i];
        if (cmd.cmd == NULL) {
            printf("bad command\n");
        } else if (strcmp(cmd.cmd, "p") == 0 && cmd.key != NULL && cmd.value != NULL) {
            // printf("PUT %s %s\n", cmd.key, cmd.value);
            bool key_ok;
            long key = str_to_long(cmd.key, 10, &key_ok);
            if (!key_ok) {
                printf("bad command\n");
                continue;
            }
            remove_entry(&entries, &entries_len, key);
            append_entry(&entries, &entries_len, key, cmd.value, strdup(cmd.arg));
        } else if (strcmp(cmd.cmd, "g") == 0 && cmd.key != NULL && cmd.value == NULL) {
            // printf("GET %s\n", cmd.key);
            bool key_ok;
            long key = str_to_long(cmd.key, 10, &key_ok);
            if (!key_ok) {
                printf("bad command\n");
                continue;
            }
            struct Entry* entry = find_entry(&entries, &entries_len, key);
            if (entry != NULL) {
                printf("%ld,%s\n", entry->key, entry->value);
            } else {
                printf("%ld not found\n", key);
            }
        } else if (strcmp(cmd.cmd, "d") == 0 && cmd.key != NULL && cmd.value == NULL) {
            // printf("DEL %s\n", cmd.key);
            bool key_ok;
            long key = str_to_long(cmd.key, 10, &key_ok);
            if (!key_ok) {
                printf("bad command\n");
                continue;
            }
            if (!remove_entry(&entries, &entries_len, key)) {
                printf("%ld not found\n", key);
            }
        } else if (strcmp(cmd.cmd, "c") == 0 && cmd.key == NULL && cmd.value == NULL) {
            // printf("CLEAR\n");
            free_entries(entries, entries_len);
            entries = NULL;
            entries_len = 0;
        } else if (strcmp(cmd.cmd, "a") == 0 && cmd.key == NULL && cmd.value == NULL) {
            // printf("ALL\n");
            for (int i = 0; i < entries_len; i++) {
                if (entries[i].entry != NULL) {
                    printf("%ld,%s\n", entries[i].key, entries[i].value);
                }
            }
        } else {
            printf("bad command\n");
        }
    }
    storage_file = fopen("kv.txt", "w");
    if (storage_file == NULL) {
        printf("cannot open storage file for writing\n");
    } else {
        store(storage_file, entries, entries_len);
        fclose(storage_file);
    }
    free_entries(entries, entries_len);
    free_cmds(cmds, cmds_len);
    return 0;
}
