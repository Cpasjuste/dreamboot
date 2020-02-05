//
// Created by cpasjuste on 03/02/2020.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "retrodream.h"
#include "uthash/utlist.h"
#include "utility.h"

int file_exists(const char *fn) {
    struct stat st;
    return (stat(fn, &st) == 0);
}

int dir_exists(const char *dir) {
    return file_exists(dir);
}

void get_dir(List *list, const char *path) {

    DIR *dir;
    struct dirent *ent;
    ListItem *entry;

    memset(list, 0, sizeof(List));
    strncpy(list->path, path, MAX_PATH - 1);

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {

            // skip "."
            if (ent->d_name[0] == '.') {
                continue;
            }

            entry = (ListItem *) malloc(sizeof *entry);
            memset(entry, 0, sizeof(ListItem));

            strncpy(entry->name, ent->d_name, MAX_PATH - 1);
            if (list->path[strlen(list->path) - 1] != '/') {
                snprintf(entry->path, MAX_PATH - 1, "%s/%s", list->path, ent->d_name);
            } else {
                snprintf(entry->path, MAX_PATH - 1, "%s%s", list->path, ent->d_name);
            }

            struct stat st;
            if (stat(entry->path, &st) == 0) {
                entry->type = S_ISDIR(st.st_mode) ? TYPE_DIR : TYPE_FILE;
            }

            DL_APPEND(list->head, entry);
            list->size++;
        }

        DL_SORT(list->head, list_cmp);
        closedir(dir);
    }
}

char *read_file(const char *file) {

    size_t size;
    FILE *fp = NULL;
    char *buffer = NULL;

    fp = fopen(file, "rb");
    if (fp == NULL) {
        printf("read_file: can't open %s\n", file);
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    buffer = (char *) malloc(size);
    memset(buffer, 0, size);

    if (fread(buffer, 1, size, fp) != size) {
        fclose(fp);
        free(buffer);
        printf("read_file: can't read %s\n", file);
        return NULL;
    }

    fclose(fp);

    return buffer;
}

int flash_get_region() {
    return 0;
}


int is_hacked_bios() {
    return 0;
}

int is_custom_bios() {
    return 0;
}

int is_no_syscalls() {
    return 0;
}

void loader_init() {
    printf("loader_init\n");
}

void dc_load_serial(void) {
    printf("dc_load_serial\n");
}

void dc_load_ip(void) {
    printf("dc_load_ip\n");
}

void exec(const char *path) {
    printf("exec(%s)\n", path);
}
