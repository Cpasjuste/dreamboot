//
// Created by cpasjuste on 03/02/2020.
//

#include <stdlib.h>
#include <string.h>
#include <uthash/utlist.h>
#include "retrodream.h"
#include "utility.h"

int list_cmp(ListItem *a, ListItem *b) {

    if (a->type == TYPE_DIR && b->type != TYPE_DIR) {
        return -1;
    } else if (a->type != TYPE_DIR && b->type == TYPE_DIR) {
        return 1;
    }

    return strcasecmp(a->name, b->name);
}

void free_dir(List *list) {

    ListItem *elt, *tmp;
    DL_FOREACH_SAFE(list->head, elt, tmp) {
        DL_DELETE(list->head, elt);
        free(elt);
    }
}

ListItem *get_item(List *list, int index) {

    ListItem *file = list->head;
    if (index == 0) {
        return file;
    }

    for (int i = 1; i < list->size; i++) {
        file = (ListItem *) file->next;
        if (index == i) {
            return file;
        }
    }

    return NULL;
}

void try_boot() {

    // first check for boot config
    if (file_exists("/ide/boot.cfg")) {
        char *path = read_file("/ide/boot.cfg");
        if (path != NULL) {
            trim(path);
            if (file_exists(path)) {
                exec(path);
            }
        }
    } else if (file_exists("/sd/boot.cfg")) {
        char *path = read_file("/sd/boot.cfg");
        if (path != NULL) {
            trim(path);
            if (file_exists(path)) {
                exec(path);
            }
        }
    }
    // then retrodream.bin
    if (file_exists("/ide/RD/retrodream.bin")) {
        exec("/ide/RD/retrodream.bin");
    } else if (file_exists("/sd/RD/retrodream.bin")) {
        exec("/sd/RD/retrodream.bin");
    }
    // finally check for DS_CORE.BIN
    if (file_exists("/ide/DS/DS_CORE.BIN")) {
        exec("/ide/DS/DS_CORE.BIN");
    } else if (file_exists("/sd/DS/DS_CORE.BIN")) {
        exec("/sd/DS/DS_CORE.BIN");
    }
}

void trim(char *str) {

    char *pos = NULL;

    while ((pos = strrchr(str, '\n')) != NULL) {
        *pos = '\0';
    }

    size_t len = strlen(str) - 1;
    for (int i = len; i; i--) {
        if (str[i] > ' ') {
            break;
        }
        str[i] = '\0';
    }
}
