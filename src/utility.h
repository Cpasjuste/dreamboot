//
// Created by cpasjuste on 28/01/2020.
//

#ifndef LOADER_UTILITY_H
#define LOADER_UTILITY_H

#define MAX_PATH 512

enum FileType {
    TYPE_DIR,
    TYPE_FILE,
    TYPE_BIN
};

typedef struct ListItem {
    struct ListItem *next, *prev;
    char name[MAX_PATH];
    char path[MAX_PATH];
    int type;
} ListItem;

typedef struct List {
    ListItem *head;
    int size;
    char path[MAX_PATH];
} List;

int list_cmp(ListItem *a, ListItem *b);

void get_dir(List *list, const char *path);

void free_dir(List *list);

ListItem *get_item(List *list, int index);

int file_exists(const char *file);

int dir_exists(const char *dir);

void try_boot();

char *read_file(const char *file, int *size);

void exec(const char *path);

int is_hacked_bios();

int is_custom_bios();

int is_no_syscalls();

int flash_get_region();

void descramble(uint8 *source, uint8 *dest, uint32 size);

int setup_syscalls();

void dc_load_serial();

void dc_load_ip();

void loader_init();

void trim(char *str);

#endif //LOADER_UTILITY_H
