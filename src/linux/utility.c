//
// Created by cpasjuste on 03/02/2020.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "cross.h"
#include "utility.h"

extern FileList s_file_list;

extern void add_file(FileList *list, File *entry, int sort);

extern FileList *get_dir(const char *path);

extern void free_dir(FileList *list);

int file_exists(const char *fn) {
    struct stat st;
    return (stat(fn, &st) == 0);
}

int dir_exists(const char *dir) {
    return file_exists(dir);
}

FileList *get_dir(const char *path) {

    DIR *dir;
    struct dirent *ent;
    File *entry;

    free_dir(&s_file_list);
    memset(&s_file_list, 0, sizeof(FileList));
    strncpy(s_file_list.path, path, MAX_PATH);

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {

            // skip "."
            if (ent->d_name[0] == '.') {
                continue;
            }

            entry = (File *) malloc(sizeof(File));
            strncpy(entry->name, ent->d_name, MAX_PATH);
            if (path[strlen(path) - 1] != '/') {
                snprintf(entry->path, MAX_PATH - 1, "%s/%s", path, ent->d_name);
            } else {
                snprintf(entry->path, MAX_PATH - 1, "%s%s", path, ent->d_name);
            }

            struct stat st;
            if (stat(entry->path, &st) == 0) {
                entry->type = S_ISDIR(st.st_mode) ? TYPE_DIR : TYPE_FILE;
            }

            add_file(&s_file_list, entry, SORT_BY_NAME_AND_FOLDER);
        }
        closedir(dir);
    }

    return &s_file_list;
}

char *read_file(const char *file) {
    return NULL;
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
